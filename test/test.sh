for i in t.*.rb; do ruby -I ../ext $i 2>&1 | lv; done
