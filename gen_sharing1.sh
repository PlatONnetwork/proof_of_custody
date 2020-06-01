#!/usr/bin/expect

# FIRST: sudo apt install expect

if {$argc < 1} {
   send_user "usage: $argv0 <t>\n"
   exit
}

set t [lindex $argv 0]
set timeout 60

spawn ./Setup.x
expect "Enter a number"
send "2\n"
expect "Enter2 a number"
send "2\n"
expect "for secret sharing"
send "16030569034403128277756688287498649515510226217719936227669524443298095169537\n"
expect "Enter threshold"
send "${t}\n"
expect off
