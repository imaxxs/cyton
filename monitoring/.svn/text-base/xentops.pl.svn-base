my $XMTOP = '/usr/sbin/xentop';
@chunks = split(/^xentop - .*$/m, `$XMTOP -b -i2 -d2`);
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
        my @v_tmp = split(" ", $domain);
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
