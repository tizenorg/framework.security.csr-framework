#!/usr/bin/perl -w

use strict;

our %funcs;

our $expectedrtn = {'TCSLIB_HANDLE' => 'INVALID_TCSLIB_HANDLE', 
                    'TWPLIB_HANDLE' => 'INVALID_TWPLIB_HANDLE',
                    'int' => '-1', 'TWP_RESULT' => 'TWP_NOT_IMPLEMENTED', 
                    'TCSErrorCode' => '0x01000007'};

sub dumptestcode {
    my ($handle) = $_[0];
    my ($rtn, $params, $expected);
    foreach my $func (sort keys %funcs){
        $rtn = $funcs{$func}{'rtn'};
        $params = $funcs{$func}{'params'};
        $expected = $expectedrtn->{$rtn};
        print $handle <<"EOT";
/**
 * \@brief Negative test case of ${func}()
 */
static void utc_csr_framework_${func}_n001(void)
{
EOT
        if ($rtn ne "void"){
            print $handle <<"EOT";
    ${rtn} rtn = (${rtn})0xdeadbeef;
    rtn = ${func}\(${params}\);
    dts_check_eq\(\"${func}\", rtn, ${expected}, "${func} returned unexpected value.");
EOT
        }else{
            print $handle <<"EOT";
    ${func}\(${params}\);
    dts_pass\(\"${func}\", "${func} passed as expected");
EOT
        }
        print $handle "}\n\n\n";
    }
}


sub dumpstub {
    my ($handle) = $_[0];

print $handle <<'EOT';
/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <tet_api.h>
#include <stdio.h>
#include <stdlib.h>

#include <TCSImpl.h>
#include <TWPImpl.h>
#include <TCSErrorCodes.h>

enum {
    POSITIVE_TC_IDX = 0x01,
    NEGATIVE_TC_IDX,
};

static void startup(void);
static void cleanup(void);

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;


EOT

    foreach my $func (sort keys %funcs){
        print $handle <<"EOT";
static void utc_csr_framework_${func}_n001(void);
EOT
    }
    print $handle "\n\n";
    print $handle "struct tet_testlist tet_testlist[] = {\n";
    foreach my $func (sort keys %funcs){
        print $handle <<"EOT";
    { utc_csr_framework_${func}_n001, NEGATIVE_TC_IDX},
EOT
    }
    print $handle "    { NULL, 0 }, \n};\n\n\n";

print $handle <<'EOT';
static void startup(void)
{
    /* start of TC */
    tet_printf("\n TC start");
}


static void cleanup(void)
{
    /* end of TC */
    tet_printf("\n TC end");
}


EOT
}

sub filldummyparams {
    my ($prototypes) = @_;
    my @params = ();
    if ($prototypes eq 'void'){
       return "";
    }
    my $i = 0;
    my @args = split(/,/, $prototypes);
    foreach my $arg (@args){
        if (index($arg, "*") > -1){
            $params[$i++] = "NULL";
        }elsif ($arg =~ /TWPLIB_HANDLE.*/){
            $params[$i++] = "INVALID_TWPLIB_HANDLE";
        }elsif ($arg =~ /TCSLIB_HANDLE.*/){
            $params[$i++] = "INVALID_TCSLIB_HANDLE";
        }elsif ($arg =~ /int.*/){
            $params[$i++] = "0";
        }elsif ($arg =~ /unsigned.*/){
            $params[$i++] = "0";
        }elsif ($arg =~ /TWPCategories.*/){
            $params[$i++] = "TWP_LastCategoryPlaceholder";
        }elsif ($arg =~ /.*Handle/){
            $params[$i++] = "NULL";
        }
    }
    return join(',', @params);
}

sub run{
    my ($handle) = @_;
    my ($rtn, $func, $args);
    while(<$handle>){
        s/[\r\n]+//g;
        next unless (/([^\s]+) ([^\s]+)\s{0,}\(([^\(\)]+)\)/);
        $rtn = $1;
        $func = $2;
        $args = $3;
        $funcs{$func}{'rtn'} = $rtn;
        $funcs{$func}{'params'} = filldummyparams($args);
    }
}
open(FILE, "< $ARGV[0]") || die "$!";
run(*FILE);
close(FILE);
dumpstub(*STDOUT);
dumptestcode(*STDOUT);

