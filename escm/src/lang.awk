# src/lang.awk - Awk version of lang.c
# $Id$
# Author: TAGA Yoshitaka <tagga@tsuda.ac.jp>

BEGIN {
  leading = "";
  key = "";
  split("name command initialization nil bind assign string expression finalization", data);
  for (x in data) {
    data[data[x]] = "";
    delete data[x];
  }
  split("name nil string expression", obl);
  for (x in obl) {
    obl[obl[x]] = "";
    delete obl[x];
  }
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
  key = "name";
}
END {
  for (x in obl) {
    if (data[x] == "") {
      print "*", x, "* field required";
      exit 1;
    }
  }
  if (!data["bind"] && !data["assign"]) {
    print "Either *bind* or *assign* field required";
      exit 1;
  }
  if (data["bind"] && !data["assign"]) {
    data["assign"] = data["bind"];
  }
  if (data["assign"] && !data["bind"]) {
    data["bind"] = data["assign"];
  }
  print "/* src/scm.c - generated by src/lang.awk */";
  print "#include \"escm.h\"";
  print "struct escm_lang lang_scm = {";
  sub("^<\\?", "", data["name"]);
  sub(" .*\\?>.*", "", data["name"]);
  print " ", escape_string(data["name"]), ", /* name */";
  if (!match(data["string"], "string")) {
    print "keyword *string* missing";
    exit 1;
  }
  print " ", escape_string(substr(data["string"], 1, RSTART - 1)), ", /* literal_prefix */";
  print " ", escape_string(substr(data["string"], RSTART + 6)), ", /* literal_suffix */";
  if (!match(data["expression"], "expression")) {
    print "keyword *expression* missing";
    exit 1;
  }
  print " ", escape_string(substr(data["expression"], 1, RSTART - 1)), ", /* display_prefix */";
  print " ", escape_string(substr(data["expression"], RSTART + 10)), ", /* display_suffix */";

  if (!match(data["bind"], "variable[-_]name")) {
    print "keyword *variable-name* or *variable_name* missing";
    exit 1;
  }
  print " ", escape_string(substr(data["bind"], 1, RSTART - 1)), ", /* bind_prefix */";
  prev = RSTART;
  if (!match(data["bind"], "value")) {
    print "keyword *value* missing";
    exit 1;
  }
  print " ", escape_string(substr(data["bind"], prev + 13, RSTART - prev - 13)), ", /* bind_infix */";
  print " ", escape_string(substr(data["bind"], RSTART + 5)), ", /* bind_suffix */";

  if (!match(data["assign"], "variable[-_]name")) {
    print "keyword *variable-name* or *variable_name* missing";
    exit 1;
  }
  print " ", escape_string(substr(data["assign"], 1, RSTART - 1)), ", /* assign_prefix */";
  prev = RSTART;
  if (!match(data["assign"], "value")) {
    print "keyword *value* missing";
    exit 1;
  }
  print " ", escape_string(substr(data["assign"], prev + 13, RSTART - prev - 13)), ", /* assign_infix */";
  print " ", escape_string(substr(data["assign"], RSTART + 5)), ", /* assign_suffix */";

  if (match(data["bind"], "variable-name") && match(data["assign"], "variable-name")) {
    print " ", 1, ", /* use_hyphen */";
  } else if (match(data["bind"], "variable_name") && match(data["assign"], "variable_name")) {
    print " ", 0, ", /* use_hyphen */";
  } else {
    print "Conflicting use of hyphens";
    exit;
  }
  print " ", escape_string(data["nil"]), ", /* nil */"
  print " ", escape_string(data["initialization"]), ", /* init */"
  print " ", escape_string(data["finalization"]), ", /* finish */"
  print "};";
  print "/* end of scm.c */";
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