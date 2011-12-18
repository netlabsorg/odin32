#!/bih/sh

#
# Odin RPM spec file updater.
#
# Project Odin Software License can be found in LICENSE.TXT
#
# NOTE: This script requires rpmbuild and the basic set of unix tools such
# as cp, mkdir, pwd, readlink, etc.
#

#
# Defaults
#

spec_file="odin.spec"

#
# Functions
#

die() { echo "ERROR: $@"; cd "$start_dir"; exit 1; }

run()
{
    echo "$@"
    "$@" || die \
"Last command failed (exit status $?)."
}

cmd_update()
{
    local have_git="$(git --version)"

    # get the version info from .spec
    local s_ver_major=$(sed -nre \
        "s/^%define ver_major[[:space:]]+([0-9]+).*$/\1/p" < "$spec_file")
    local s_ver_minor=$(sed -nre \
        "s/^%define ver_minor[[:space:]]+([0-9]+).*$/\1/p" < "$spec_file")
    local s_ver_patch=$(sed -nre \
        "s/^%define ver_patch[[:space:]]+([0-9]+).*$/\1/p" < "$spec_file")
    local s_rpm_release=$(sed -nre \
        "s/^%define rpm_release[[:space:]]+([0-9]+).*$/\1/p" < "$spec_file")

    [ -z "$s_ver_major" -o -z "$s_ver_minor" -o -z "$s_ver_patch" -o \
      -z "$s_rpm_release" ] && \
        die "Could not determine version number in '$spec_file'."

    echo
    echo "Old version: $s_ver_major.$s_ver_minor.$s_ver_patch-$s_rpm_release"

    if [ "$s_ver_major.$s_ver_minor.$s_ver_patch" != \
         "$ver_major.$ver_minor.$ver_build" ]; then
        # reset the revision number and take the new version number
        s_rpm_release=1
        s_ver_major=$ver_major
        s_ver_minor=$ver_minor
        s_ver_patch=$ver_build
    else
        # increase the current revision number
        s_rpm_release=$(($s_rpm_release + 1))
    fi

    echo "New version: $s_ver_major.$s_ver_minor.$s_ver_patch-$s_rpm_release"

    # Get the user name and email from .gitconfig, if any
    if [ -n "$have_git" ]; then
        packager_name=$(git config --get user.name)
        packager_email=$(git config --get user.email)
    fi

    if [ -z "$packager_name" -a -z "$packager_email" ]; then
        packager="Your Name <your/email.com>"
    elif [ -z "$packager_name" ]; then
        packager="<$packager_email>"
    elif [ -z "$packager_email" ]; then
        packager="$packager_name"
    else
        packager="$packager_name <$packager_email>"
    fi

    echo "Packager:    $packager"

    # Get the current date in the required format
    cur_date=`LANG=C date "+%a %b %d %Y"`

    echo "Date:        $cur_date"
    echo

    # backup the original .spec file
    run cp -Rdp "$spec_file" "$spec_file.old"

    # escape some values for sed
    packager=$(echo "$packager" | sed -re 's/[\\\/\&]/\\&/g')

    # update it with new data (note that we pipe through tr to remove CR since
    # sed uses CRLF for line terminators on OS/2 but rpmbuild can't stand it)
    echo 'sed ... < '$spec_file.old' > '$spec_file
    sed -re '
{
    s/^(%define ver_major[[:space:]]+)[0-9]+(.*)$/\1'$s_ver_major'\2/
    s/^(%define ver_minor[[:space:]]+)[0-9]+(.*)$/\1'$s_ver_minor'\2/
    s/^(%define ver_patch[[:space:]]+)[0-9]+(.*)$/\1'$s_ver_patch'\2/
    s/^(%define rpm_release[[:space:]]+)[0-9]+(.*)$/\1'$s_rpm_release'\2/

    s/^%changelog[[:space:]]*$/%changelog\n\n\* '"$cur_date"' '"$packager"\
' - '"$s_ver_major"'.'"$s_ver_minor"'.'"$s_ver_patch"'-'"$s_rpm_release"\
'\n- <List changes here>/
}
' < "$spec_file.old" | tr -d '\015' > "$spec_file"

    rc=$?
    if [ "$rc" != "0" ]; then
        echo "sed command failed (exit status $rc)."
        cp -Rdp "$spec_file.old" "$spec_file"
        rm "$spec_file.old"
        exit $rc
    fi

    echo "ALL DONE."
}

#
# Main
#

script_path=$(readlink -f $0)
script_dir=${script_path%/*}

start_dir=$(pwd)

# Check prerequisites

[ -f "$spec_file" ] || \
    die "Could not find file '$spec_file'."

# Get Odin version number

odin_root="$script_dir/../.."

odinbuild_h="$odin_root/include/odinbuild.h"

[ -f "$odinbuild_h" ] || \
    die "Could not find file '$odinbuild_h'."

ver_major=$(sed -nre \
    "s/^#define ODIN32_VERSION_MAJOR[[:space:]]+([0-9]+).*$/\1/p" < "$odinbuild_h")
ver_minor=$(sed -nre \
    "s/^#define ODIN32_VERSION_MINOR[[:space:]]+([0-9]+).*$/\1/p" < "$odinbuild_h")
ver_build=$(sed -nre \
    "s/^#define ODIN32_BUILD_NR[[:space:]]+([0-9]+).*$/\1/p" < "$odinbuild_h")

[ -z "$ver_major" -o -z "$ver_minor" -o -z "$ver_build" ] && \
    die "Could not determine Odin version number in '$odinbuild_h'."

ver_dots="$ver_major.$ver_minor.$ver_build"

# Parse arguments

cmd_help()
{
    echo \
"
Usage:
  $0 update     Update the \"$spec_file\" file.
"
}

case $1 in
    "--help" | "-?" | "-h" | "") cmd_help;;
    "update") cmd_update;;
esac

# end of story

exit 0

