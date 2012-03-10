/cleaning/ {
    p
    b eof
}
/including/ {
    p
    b eof
}
/depending/ {
    p
    b eof
}
/making/ {
    p
    b eof
}
/installing/ {
    p
    b eof
}
/{^A-Za-z]make:/ {
    p
    b eof
}
/{^A-Za-z]ld:/ {
    p
    b eof
}
/{^A-Za-z]cc:/ {
    p
    b eof
}
/{^A-Za-z]cp:/ {
    p
    b eof
}
/{^A-Za-z]gencat:/ {
    p
    b eof
}
/{^A-Za-z]mv:/ {
    p
    b eof
}
/{^A-Za-z]rm:/ {
    p
    b eof
}
/[^-._A-Za-z][Ee][Rr][Rr][Oo][Rr][^-._A-Za-z]/ {
    p
    b eof
}
/couldn't set locale/h
/making/g
:eof
