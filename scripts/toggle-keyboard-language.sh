#!/bin/sh
current=$(xkb-switch -p)

case "$current" in
us) xkb-switch -s es ;;
es) xkb-switch -s us ;;
*) xkb-switch -s us ;;
esac
