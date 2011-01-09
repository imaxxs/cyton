#!/usr/bin/perl -w
# Xen Stats - Xen  guest (dom-u) stats
# Copyright (C) 2007-2009 ZZ Servers
# www.zzservers.com
# Tested with Xen 3.1

# TODO:
# 1 - Get actual memory usage details
# apt-get install libxml-csv-perl


use LWP::UserAgent; 
use Shell;
use LockFile::Simple qw(lock trylock unlock);
use XML::CSV;

# This servers details.
my $SERVER_ID     = "";
my $SERVER_KEY     = "";

# URL for server for xen_stats to post data
# full url http://www.domain.com https://www.domain.com
my $SERVER_URL     = "";

# User agent, shows up in web logs when xen_stats posts to server
my $AGENT         = "";

# Leave alone unless you are going through a web proxy
my $PROXY         = "";

# Define where to find xm tools
my $XM             = '/usr/sbin/xm';
my $XMTOP         = '/usr/sbin/xentop';

##############
# You should not need to edit anything below here

$ENV{PATH} = '/bin:/usr/bin:/usr/sbin';

# Lock, otherwise quit as another version is running
trylock("/tmp/xen_stats.run") || die "DCM Queue still running, can't lock /tmp/xen_stats.run\n";

# Unlock and exit
sub quitnow {
        unlock("/tmp/xen_stats.run");
        exit 0;
}
my @chunks; undef(@chunks);

# run the xentop command a few times because the first reading is not always accurate
# xentop has 20 seconds to run before timing out
eval {
    local $SIG{ALRM} = sub { die "TIMEDOUT" };
    alarm(20);

    @chunks = split(/^xentop - .*$/m, `$XMTOP -b -i2 -d2`);
    alarm(0);
};
if ($@) {
    die $@ unless $@ =~ /TIMEDOUT/;
    # command timed out
    # Run command to post failure to management server
    print "Timeout";
} else {
    # Xen is online, get stats
    # Take only the last run of xentop
    my @stats = split (/\n/,pop(@chunks));

    # remove the first 4 items that are junk that we don't need.
    shift(@stats); 
    shift(@stats); 
    shift(@stats); 
    shift(@stats); 

    my %vals; undef(%vals);

    foreach my $domain (@stats) {
        # trim the leading whitespace and break into an array for later use
        $domain =~ s/^\s+//;
        my @v_tmp = split(/\s+/, $domain);
        # $v_tmp[0] - name
        # $v_tmp[1] - state
        # $v_tmp[2] - cpu_sec
        # $v_tmp[3] - cpu_pct
        # $v_tmp[4] - mem
        # $v_tmp[5] - mem_pct
        # $v_tmp[6] -max_mem
        # $v_tmp[7] - max_mem_pct
        # $v_tmp[8] - vcpus
        # $v_tmp[9] - nets
        # $v_tmp[10] -nettx
        # $v_tmp[11] - netrx
        # $v_tmp[12] - vbds
        # $v_tmp[13] - vbd_oo
        # $v_tmp[14] - vbd_rd
        # $v_tmp[15] - vbd_wr
        # $v_tmp[16] - ssid

        my $DOMAIN = $v_tmp[0];
        my $hostname=$v_tmp[0];
        open(VMS, '/usr/local/etc/host-list');
        while (<VMS>) {
          chomp;
          my ($name, $xensourcename) = split / /;
          if ($xensourcename =~ m/$hostname/) {
            $v_tmp[0]=$name;
          }
        }
        close(VMS);
    
        # to get Domain-0 details need to shift all array items after maxmem(k) if no max memory limit is set.
        if ($v_tmp[6] eq "no") {
            $v_tmp[6] = $v_tmp[7];
            $v_tmp[7] = $v_tmp[8];
            $v_tmp[8] = $v_tmp[9];
            $v_tmp[9] = $v_tmp[10];
            $v_tmp[10] = $v_tmp[11];
            $v_tmp[11] = $v_tmp[12];
            $v_tmp[12] = $v_tmp[13];
            $v_tmp[13] = $v_tmp[14];
            $v_tmp[14] = $v_tmp[15];
            $v_tmp[15] = $v_tmp[16];
            $v_tmp[16] = $v_tmp[17];
        }
    
        # Get uptime details
        #$dev_tmp[2] eq total days up
        # $dev_tmp[4] eq total hours up
        # if $dev_tmp[4] not defined, server online less than 1 day and $dev_tmp[2] now eq total hours up
        # make $dev_tmp[4] = $dev_tmp{2] and set $dev_tmp{2] to 0
        my @dev; undef(@dev);
        my @dev_tmp; undef(@dev_tmp);

        eval {
            local $SIG{ALRM} = sub { die "TIMEDOUT" };
            alarm(5);
            @dev=grep (/$hostname/, `xm uptime $hostname`);
            alarm(0);
        };

        if ($@) {
            die $@ unless $@ =~ /TIMEDOUT/;
            # command timed out


            $dev_tmp[4] = 0;
            $dev_tmp[2] = 0;
        } else {
            @dev_tmp = split(/\s+/, $dev[0]);
            # 
            if (defined($dev_tmp[4])) {
            } else {
                $dev_tmp[4] = $dev_tmp[2];
                $dev_tmp[2] = 0;
            }
        }
        
        $vals{$v_tmp[0]}{'days_uptime'}         = $dev_tmp[2];
        $vals{$v_tmp[0]}{'hours_uptime'}         = $dev_tmp[4];
        $vals{$v_tmp[0]}{'state'}                 = $v_tmp[1];
        $vals{$v_tmp[0]}{'cpu_seconds'}         = $v_tmp[2];
        $vals{$v_tmp[0]}{'cpu_percent'}         = $v_tmp[3];
        $vals{$v_tmp[0]}{'memory'}                 = $v_tmp[4];
        $vals{$v_tmp[0]}{'memory_percent'}         = $v_tmp[5];
        $vals{$v_tmp[0]}{'max_memory'}             = $v_tmp[6];
        $vals{$v_tmp[0]}{'max_meory_percent'}     = $v_tmp[7];
        $vals{$v_tmp[0]}{'vcpu'}                 = $v_tmp[8];
        $vals{$v_tmp[0]}{'nets'}                 = $v_tmp[9];
        $vals{$v_tmp[0]}{'nettx'}                 = $v_tmp[10] * 1024;
        $vals{$v_tmp[0]}{'netrx'}                 = $v_tmp[11] * 1024;
        $vals{$v_tmp[0]}{'vdbs'}                 = $v_tmp[12];
        $vals{$v_tmp[0]}{'vbd_oo'}                 = $v_tmp[13];
        $vals{$v_tmp[0]}{'vbd_rd'}                 = $v_tmp[14];
        $vals{$v_tmp[0]}{'vbd_wr'}                 = $v_tmp[15];


        $vals{$v_tmp[0]}{'ssid'}                 = $v_tmp[16];

    }

    # Build arrays containing current snapshot details for each domain
    #    my %host; undef(%host);
    open FILE, "+>/tmp/host_snapshot.csv" or die $!;


    foreach my $key (sort(keys(%vals))) {
    
        print FILE "$key,$vals{$key}{'cpu_percent'},$vals{$key}{'nettx'},$vals{$key}{'netrx'},$vals{$key}{'days_uptime'}:$vals{$key}{'hours_uptime'},$vals{$key}{'state'},$vals{$key}{'cpu_seconds'},$vals{$key}{'memory'},$vals{$key}{'memory_percent'},$vals{$key}{'max_memory'},$vals{$key}{'max_meory_percent'},$vals{$key}{'vcpu'},$vals{$key}{'nets'},$vals{$key}{'vdbs'},$vals{$key}{'vbd_oo'},$vals{$key}{'vbd_rd'},$vals{$key}{'vbd_wr'},$vals{$key}{'ssid'}\n";


    }
    close FILE;

    my $file = '/tmp/host_snapshot.csv';
    my @columns = ('host', 'cpu_percent', 'nettx', 'netrx', 'uptime', 'state', 'cpu_seconds', 'memory', 'memory_percent', 'max_memory', 'max_meory_percent','vcpu', 'nets','vdbs','vbd_oo','vbd_rd','vbd_wr','ssid');
    my $csv = XML::CSV->new({column_headings => \@columns});
    $csv->parse_doc($file, {column_headings => \@columns});
#    $csv->declare_xml({version => '1.0',
#                       standalone => 'yes'});
    $csv->print_xml('/tmp/host_snapshot.xml');

    open FILE, "</tmp/host_snapshot.xml" or die $!;
    while (<FILE>) { $lines .= $_; }    
    $hosts{'host_xml'}     = $lines;
    $hosts{'sid'}         = $SERVER_ID;
    $hosts{'action'}     = "stats";

    my $ua = LWP::UserAgent->new;
    $ua->timeout(25);
    $ua->agent($AGENT);
    $ua->proxy(['http'], $PROXY) if defined $PROXY;

    my $response = $ua->post(
                        $SERVER_URL,
                        Content => \%hosts);

    my $content = $response->content; 
    print $content;
}
quitnow();
