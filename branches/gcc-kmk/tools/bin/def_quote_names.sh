#!/bin/sh

#
# Quotes funcion names in .DEF files that contain characters like '@'.
# This is needed for GCC .DEF parser which doesn't accept them otherwise.
#
# Usage: def_quote_names.sh <input.def> [<output.def>]
#
# If not specified, output goes to "<input.def>.new".
#

IN_DEF="$1"
OUT_DEF="$2"

[ -z "$IN_DEF" ] && { echo "Must specify input .DEF file."; exit 1; }
[ -z "$OUT_DEF" ] && OUT_DEF="$IN_DEF.new"

# chars that require quoting
Q='@\\?'

SP='[[:space:]]'
AZ_='[A-Za-z_]'
AZ09_='[A-Za-z0-9_]'
AZ09_Q='[A-Za-z0-9_'$Q']'

AZ09_WITH_Q=$AZ_$AZ09_Q'*'$Q'+'$AZ09_Q'*'

REMINDER='(('$SP'+@[0-9]+('$SP'+NONAME)?)?('$SP'*;.*)?'$SP'*)'

sed -r '
{
  # foo = bar@0       =>     foo = "bar@0"
  s/^('$SP'*)("?'$AZ_$AZ09_'*"?'$SP'*='$SP'*)(('$AZ09_WITH_Q')('$SP$SP')?)'$REMINDER'$/\1\2"\4"\6/
  # foo@0 = bar@0     =>     "foo" = "bar@0"
  s/^('$SP'*)(('$AZ09_WITH_Q')('$SP$SP')?)('$SP'*='$SP'*)(('$AZ09_WITH_Q')('$SP$SP')?)'$REMINDER'$/\1"\3"\5"\7"\9/
  # foo@0 = bar       =>     "foo" = bar
  # foo@0             =>     "foo"
  s/^('$SP'*)(('$AZ09_WITH_Q')('$SP$SP')?)('$SP'*='$SP'*"?'$AZ_$AZ09_'*"?)?'$REMINDER'$/\1"\3"\5\6/
}
' < "$IN_DEF" > "$OUT_DEF"

#