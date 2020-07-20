# Exercise source: https://linux.die.net/Bash-Beginners-Guide/sect_06_05.html

# For the this exercise, your input is lines in the following form:
# Username:Firstname:Lastname:Telephone number
#
# Make an awk script that will convert such a line to an LDAP record in this
# format:
# dn: uid=Username, dc=example, dc=com
# cn: Firstname Lastname
# sn: Lastname
# telephoneNumber: Telephone number
#
# Create a file containing a couple of test records and check.

BEGIN {
    FS = ":";
}

{
    print "dn: uid=" $1 ", dc=example, dc=com";
    print "cn: " $2 " " $3;
    print "sn: " $3;
    print "telephoneNumber: " $4;
    print "";
}
