# Added this alias with the assistance of gemini

alias gpaste='xclip -selection clipboard -o >'
# This was supposed to work on wayland but it did not work
# on this machine.
# alias copy='wl-copy'
# The one below did work
alias copy='xclip -selection clipboard'