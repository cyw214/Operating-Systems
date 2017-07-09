# type '. tools.sh' to add gcc6.2 to your path

# Add gcc-6.2 to the front of the path, so it comes before the default gcc
export PATH=/proj/spear0/gcc-6.2/bin:/proj/spear0/gdb-7.11.1/bin:$PATH
# Add gcc's run-time libraries
export LD_LIBRARY_PATH=/proj/spear0/gcc-6.2/lib64:/proj/spear0/gcc-6.2/lib:/proj/spear0/gdb-7.11.1/lib:$LD_LIBRARY_PATH

# disable some password request popups when X-forwarding
unset SSH_ASKPASS

# Installation Notes
#   GCC
#     Source Files:        gcc-6.2.0.tar.bz2
#     Configuration Flags: --prefix=/proj/spear0/gcc-6.2 --enable-languages=c,c++,lto,go \
#                          --enable-lto --enable-multilib
#
#   GDB
#     Source Files:        gdb-7.11.1.tar.gz
#     Configuration Flags: --prefix=/proj/spear0/gdb-7.11.1 --enable-tui
