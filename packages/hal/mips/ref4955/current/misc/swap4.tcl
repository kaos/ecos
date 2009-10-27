#!/usr/bin/env tclsh

proc filter { input_file output_file } {
    set input_fd [open $input_file  "r"]
    set output_fd  [open $output_file "w"]

    fconfigure $input_fd  -translation binary
    fconfigure $output_fd -translation binary
    while { 1 } {
	set data [read $input_fd 4]
	if { [eof $input_fd] } {
	    break
	}
	binary scan $data "i" var
	puts -nonewline $output_fd [binary format "I" $var]
    }
    close $input_fd
    close $output_fd
}

filter [lindex $argv 0] [lindex $argv 1]
