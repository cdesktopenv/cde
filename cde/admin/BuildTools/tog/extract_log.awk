
BEGIN {
	do_print = 0
}
/making Makefiles in / {
	pos = match($NF, PATTERN)
	if (pos == 1) {
	    do_print = 1
            print
	    next
	}
	else {
	    do_print = 0
	    next
	}
}
/cleaning in / {
	pos = match($NF, PATTERN)
	if (pos == 1) {
	    do_print = 1
            print
	    next
	}
	else {
	    do_print = 0
	    next
	}
}
/including in / {
	pos = match($NF, PATTERN)
	if (pos == 1) {
	    do_print = 1
            print
	    next
	}
	else {
	    do_print = 0
	    next
	}
}
/depending in / {
	pos = match($NF, PATTERN)
	if (pos == 1) {
	    do_print = 1
            print
	    next
	}
	else {
	    do_print = 0
	    next
	}
}
/making All in / {
	pos = match($NF, PATTERN)
	if (pos == 1) {
	    do_print = 1
            print
	    next
	}
	else {
	    do_print = 0
	    next
	}
}
/.*/ {
	if (do_print) print
}
