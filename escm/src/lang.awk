# src/lang.awk - Awk version of lang.c
# $Id$
# Author: TAGA Yoshitaka <tagga@tsuda.ac.jp>

BEGIN {
  leading = "";
  key = "";
  split("name command init nil bind assign string display format finish", data);
  for (x in data) {
    data[data[x]] = "";
    delete data[x];
  }
  split("name string assign", obl);
  for (x in obl) {
    obl[obl[x]] = "";
    delete obl[x];
  }
  sep = "@[A-Z][A-Z][A-Z]@";
}
$1 == leading {
  key = $2;
  if (! (key in data)) {
    printf "Unknown key: %s\n", $2;
    exit 1;
  }
}
$1 != leading && key {
  if (data[key]) {
    data[key] = data[key] "\n" $0;
  } else {
    data[key] = $0
  }
}
!leading { # set the leading string
  leading = $1;
  data["name"] = $2;
  key = "command";
}
END {
  for (x in obl) {
    if (data[x] == "") {
      print "*", x, "* field required";
      exit 1;
    }
  }
  if (!data["bind"]) {
    data["bind"] = data["assign"];
  }
  if (data["nil"] == "") {
    data["nil"] = "\"\"";
  }

  print "/* src/backend.c - generated by src/lang.awk */";
  print "#include \"escm.h\"";
  print "struct escm_lang deflang = {";
  print " ", escape_string(data["name"]), ", /* name */";

  if (data["name"] == "scm" || data["name"] == "scheme") {
    print " 1 , /* scm_p */";
  } else {
    print " 0 , /* scm_p */";
  }

  print " { /* literal */";
  if (split(data["string"], tmp, sep) != 2) {
    print "string record broken";
    exit 1;
  }
  print "  ", escape_string(tmp[1]), ",";
  print "  ", escape_string(tmp[2]), ",";
  print " },";

  print " { /* display */";
  n = split(data["display"], tmp, sep);
  if (n != 0 && n != 2) {
    print "display record broken";
    exit 1;
  }
  print "  ", escape_string(tmp[1]), ",";
  print "  ", escape_string(tmp[2]), ",";
  print " },";

  print " { /* format */";
  n = split(data["format"], tmp, sep);
  if (n != 0 && n != 3) {
    print "format record broken";
    exit 1;
  }
  print "  ", escape_string(tmp[1]), ",";
  print "  ", escape_string(tmp[2]), ",";
  print "  ", escape_string(tmp[3]), ",";
  print " },";

  if (split(data["bind"], tmp, sep) != 3) {
    print "bind record broken";
    exit 1;
  }
  print " { /* bind */";
  print "  ", escape_string(tmp[1]), ",";
  print "  ", escape_string(tmp[2]), ",";
  print "  ", escape_string(tmp[3]), ",";
  print " },";

  if (split(data["assign"], tmp, sep) != 3) {
    print "assign record broken";
    exit 1;
  }
  print " { /* assign */";
  print "  ", escape_string(tmp[1]), ",";
  print "  ", escape_string(tmp[2]), ",";
  print "  ", escape_string(tmp[3]), ",";
  print " },";
  print " ", escape_string(data["nil"]), ", /* nil */"
  print " ", escape_string(data["init"]), ", /* init */"
  print " ", escape_string(data["finish"]), ", /* finish */"

  print "};";
  print "/* end of backend.c */";
}
function escape_string(str) {
  if (str == "") {
    str = "NULL";
  } else {
    gsub("\n", "\\n", str);
    gsub("\\", "\\\\", str);
    gsub("\"", "\\\"", str);
    str = "\"" str "\"";
  }
  return str;
}
