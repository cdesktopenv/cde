
#
# online feature set specification
#  qfc 12-6-94
# modified for dtinfo
#  cso 12-7-95
# modified to support tables (especially CALS tables)
#  cso 3-21-96
# removed activecharset
#  stevew 6-28-96
# modified to support vjustify in row
#  kamiya 7-12-96
# modified to support colsep and rowsep in table features
#  kamiya 7-31-96
# table renamed as tgroup
#  kamiya 8-19-96
# add *real* table
#  kamiya 8-20-96
# removed graphic, add charalign to tgroup
#  kamiya 8-28-96
# removed cols of colformat, add colstart/colend in cell
#  kamiya 8-29-96
#

border {
	display   : string;
	thickness : integer;
}

cell {
	charalign : string;
	colref    : string;
	colstart  : string;
	colend    : string;
	colsep    : integer;
	rowsep    : integer;
	morerows  : integer;
	justify   : string;
	vjustify  : string;
}

colformat {
	charalign : string;
	colsep    : integer;
	justify   : string ("left", "right", "center", "char");
	name      : string;
	rowsep    : integer;
	width     : integer;
}

font    {
        fallback : string;
	position : string ("sub", "super", "baseline", "subscript",
				"superscript");
        size : integer, dimension_pixel;
        slant: string ("roman", "italic");
	spacing : string ("char", "prop");
	style : string;
        weight : string ("medium", "bold");
	width : string ("narrow", "normal");
	family {
        	name : string;
		charset : string;
		foundry : string;
	}
}

highlight {
	bgcolor : string;
	fgcolor : string;
	overline: boolean;
	strikethrough: boolean;
	underline: boolean;
}

ignore : boolean;

layout {
	aspace	: integer;
	bspace	: integer;
	leading	: integer;
	findent	: integer;
	lindent	: integer;
	rindent	: integer;
	flow	: string ("verbatim", "filled");
	justify	: string ("left", "right", "center");
	wrap	: string ("block", "join", "none");
}

linebreak : string ("before", "after", "both");

margin  {
        left : integer;
        right : integer;
        top : integer;
        bottom : integer;
}

position {
	horiz : string ("lcorner", "left", "lmargin", "rcorner", "right",
			"rmargin", "center");
	vert : string ("top", "bottom", "middle");
}

prefix{
        content : string;
        *;
}

row {
	rowsep   : integer;
	vjustify : string;
}

suffix{
        content : string;
        *;
}

table {
	colsep : integer;
	rowsep : integer;
	frame  : string;
}

tgroup	{
	charalign : string;
	cols      : integer;
	colsep    : integer;
	justify   : string;
	rowsep    : integer;
	vjustify  : string;
}


