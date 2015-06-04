proj_dir=$(basename $(dirname $scriptdir))

THE_REV=$(git rev-parse --short HEAD)
PKG_TIMESTAMP="$(date +%Y%m%d%H%M)"

PACKAGE_SUFFIX="${THE_REV}.${PKG_TIMESTAMP}"

mkdir -p $scriptdir/../../dist
cd $scriptdir/../../dist

tmp_dir=$(mktemp -d ./tmp.XXXXXXXXXX)
pkg_dir=$tmp_dir/${APP_NAME}.$PACKAGE_SUFFIX

mkdir $pkg_dir
mkdir $pkg_dir/bin
mkdir $pkg_dir/lib

for f in "${BINARIES[@]}"; do
    cp ../$proj_dir/bin/$f $pkg_dir/bin/
done

for f in "${SCRIPTS[@]}"; do
    script_full_path="../$proj_dir/scripts/$f"
    file $script_full_path | grep -q "Bourne-Again shell script" && { bash -n $script_full_path 2>&1 || exit 1; }
    cp $script_full_path $pkg_dir/
done

for f in "${LIBS[@]}"; do
    cp ../$proj_dir/lib/$f $pkg_dir/lib/
done

for f in "${MISC[@]}"; do
    cp -a ../$proj_dir/$f $pkg_dir/
done

ln -s ${APP_NAME}.$PACKAGE_SUFFIX $tmp_dir/${APP_NAME}
tar -C $tmp_dir -zcvf ${APP_NAME}.$PACKAGE_SUFFIX.tgz ${APP_NAME}.$PACKAGE_SUFFIX ${APP_NAME}
rm -rf $tmp_dir

echo "${APP_NAME}.$PACKAGE_SUFFIX.tgz" > "${APP_NAME}.last"
