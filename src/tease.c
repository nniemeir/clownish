#include "tease.h"
#include "context.h"
#include "error.h"
#include "file.h"

int teasing_enabled = 1;

void tease_roll(struct repl_ctx *current_ctx, unsigned int command_index) {
  const int random_num = rand() % 101;
  if (random_num <= 20) {
    tease_distro();
    return;
  }
  if (random_num <= 40) {
    tease_program(current_ctx->commands[command_index][0]);
    return;
  }
  if (random_num <= 60) {
    tease_kernel();
    return;
  }
  if (random_num <= 80) {
    tease_terminal();
    return;
  }
  tease_desktop();
}

bool program_is_blacklisted(const char *program_name) {
  static const struct joke blacklisted_programs[NUM_OF_BLACKLISTED_PROGRAMS] = {
      {"emacs", "No, use vim."},
      {"neofetch", "Do we really need another minimalist cyberpunk anime "
                   "girl tiling WM rice?"},
      {"surf", "Use a real web browser."}};

  for (int i = 0; i < NUM_OF_BLACKLISTED_PROGRAMS; i++) {
    if (strcmp(program_name, blacklisted_programs[i].name) == 0) {
      printf("%s\n", blacklisted_programs[i].comment);
      return true;
    }
  }
  return false;
}

void handle_teasing(struct repl_ctx *current_ctx) {
  static bool teasing_current_command = true;
  for (unsigned int i = 0; i < current_ctx->commands_count; i++) {
    if (teasing_enabled && current_ctx->commands[i][0][0] != '\0') {
      if (teasing_current_command) {
        tease_roll(current_ctx, i);
      }
      teasing_current_command = !teasing_current_command;
    }
  }
}

void joke_binary_search(const struct joke *known_x, unsigned int array_size,
                        const char *target) {
  unsigned int lower_bound = 0;
  unsigned int upper_bound = array_size - 1;
  unsigned int match_found = 0;
  int middle_value;
  while (!match_found) {
    if (lower_bound > upper_bound) {
      middle_value = -1;
      break;
    }

    middle_value = (lower_bound + upper_bound) / 2;
    int comparison_result = strcmp(known_x[middle_value].name, target);

    if (comparison_result < 0) {
      lower_bound = middle_value + 1;
      continue;
    }

    if (comparison_result == 0) {
      match_found = 1;
      break;
    }

    if (comparison_result > 0) {
      upper_bound = middle_value - 1;
      continue;
    }
  }

  if (match_found) {
    printf("%s\n", known_x[middle_value].comment);
  }
}

// XDG_CURRENT_DESKTOP has been part of the Desktop Entry Specification since
// version 1.2 (2017), but is not universally respected (See Sway WM PR #4876)
void tease_desktop(void) {
  static int called = 0;
  if (called == 1) {
    return;
  }
  called = 1;

  char *desktop = getenv("XDG_CURRENT_DESKTOP");
  if (!desktop) {
    error_msg(env_fail_msg, false);
    return;
  }

  // TO DO - Confirm XDG_CURRENT_DESKTOP values for each of these
  static const struct joke known_desktops[NUM_OF_KNOWN_DESKTOPS] = {
      {"2bwm", "2bwm? How many times have you had to recompile it today?"},
      {"9wm", "9wm? Looks like Plan 9 while actually being useful."},
      {"afterstep", "Afterstep? My nonexistent eyes are bleeding."},
      {"awesome", "Dear Ritchie, you configure your window manager in LUA?"},
      {"berry", GENERIC_XWM_MSG},
      {"bspwm", "bspwm, so minimal it doesn't handle keybindings."},
      {"budgie", "Budgie? Oh right, I remember it now."},
      {"cde", "CDE? Can you help me fix my walkman?"},
      {"cinnamon",
       "Oh, you're on Cinnamon. How's your first week in linux going?"},
      {"COSMIC", "COSMIC and clowniSH? You must really like alpha software."},
      {"cutefish", "CUTEFISH LIVES."},
      {"deepin", "Deepin? Bold choice trusting them with your data."},
      {"enlightenment",
       "Wow, Enlightenment? That's a name I haven't heard in a long time..."},
      {"exwm", "Emacs is a fine desktop environment, maybe someday IT WILL GET "
               "A GOOD TEXT EDITOR."},
      {"frankenwm", "FrankenWM, because DWM is just too mainstream."},
      {"GNOME-Flashback", "You know MATE exists, right?"},
      {"gnome", "How is the search for GNOME extensions that haven't been "
                "deprecated going?"},
      {"hackmatrix", "Oh my, a 3D window manager? That must make you so much "
                     "more productive."},
      {"herbstluftwm", GENERIC_MANUAL_TILER_MSG},
      {"hyprland", "Are the fancy animations worth the instability?"},
      {"i3", GENERIC_MANUAL_TILER_MSG},
      {"icewm", GENERIC_MANUAL_TILER_MSG},
      {"jwm", "jwm? that is acceptable."},
      {"kde", "A KDE user? I'm sure you'll need access to all those options."},
      {"leftwm", "A Rust-based WM, how trendy."},
      {"lxde", "Move to LXQT already."},
      {"lxqt",
       "LXQT, 90%% of the memory usage of XFCE with 20%% of the features."},
      {"MATE", "MATE? The 2000s were pretty great, weren't they?"},
      {"maxxdesktop", "Do you miss your old SGI Indigo?"},
      {"niri", "A wayland compositor written in Rust? How hip, how modern."},
      {"openbox", "Did you know that some floating WMs actually let you snap "
                  "windows with your mouse?"},
      {"pekwm", GENERIC_XWM_MSG},
      {"plainDE", GENERIC_XWM_MSG},
      {"qtile", "A WM in Python? Why?"},
      {"ratpoison", "Go on, tell me all about how your WM helps you avoid RSI. "
                    "I care immensely."},
      {"spectrwm",
       "You can probably spare more than 20MB of memory for your WM."},
      {"stumpwm", GENERIC_XWM_MSG},
      {"sway", "You just can't let go of i3, can you?"}, // CONFIRMED
      {"tde", GENERIC_XWM_MSG},
      {"theDesk", GENERIC_XWM_MSG},
      {"tinywm", "Are you sure this shell is minimal enough for you?"},
      {"trinity", GENERIC_XWM_MSG},
      {"ukui",
       "Your desktop environment looks like a movie mock-up of Windows 7."},
      {"Unity", "Unity? It's okay, you can move on."},
      {"windowmaker",
       "You don't have to use Window Maker, this isn't PS2 Linux."},
      {"wingo", "wingo? Don't you know its in maintenance mode?"},
      {"worm", "No keyboard mapper?"},
      {"xfce", "XFCE? Booooring"},
      {"xmonad", "Did you learn a whole programming language just to configure "
                 "your WM?"}};
  joke_binary_search(known_desktops, NUM_OF_KNOWN_DESKTOPS, desktop);
}

// os-release NAME values sourced from which-distro's collection
void tease_distro(void) {
  static int called = 0;
  if (called == 1) {
    return;
  }
  called = 1;
  char *os_release = read_file("/etc/os-release");
  os_release = os_release + 6;
  char *second_quote = strstr(os_release, "\"");
  *second_quote = '\0';
  static const struct joke known_distros[NUM_OF_KNOWN_DISTROS] = {
      {"AlmaLinux", "AlmaLinux? Too cheap for a RHEL license?"},
      {"Alpine Linux", "Was Arch not minimal enough for you?"},
      {"Amazon Linux",
       "I'm sure Amazon will get around to releasing a new version soon."},
      {"Arch Linux 32",
       "Error: 32 bit distro detected. user is unable to let go of the past."},
      {"Arch Linux", "I bet you used archinstall you plebian."},
      {"Archcraft",
       "I'm sure you've supported Archcraft by getting premium, right?"},
      {"Arkane Linux", "\"Opinionated, immutable, atomic, minimal,\". That's a "
                       "lot of buzzwords."},
      {"Artix Linux", "Arch, for the opinionated user."},
      {"Aurora", "Yet another atomic Fedora distro."},
      {"Bazzite", "Bazzite? All of the inconvenience of silverblue with all "
                  "the productivity of Steam OS."},
      {"BlackArch Linux", "A maximalist Arch distro if there ever was one"},
      {"blendOS", "Blend? Basically just Vanilla OS."},
      {"Bluefin", "Yet another atomic Fedora distro."},
      {"CentOS Stream", "Brave of you to run beta RHEL."},
      {"Chimera", "Chimera? I hope you have a few hours to setup dual boot."},
      {"Clear Linux OS", "This Linux distribution is sponsored by Intel."},
      {"Debian GNU/Linux", "Debian? I miss two years ago too."},
      {"Deepin", "Wǒmen xīnzhōng de hóng tàiyang"},
      {"Devuan GNU/Linux", "You must be a bad sysvinit fan."},
      {"DragonFly", "Dragonfly BSD? I hope you never need nouveau."},
      {"elementary OS", "Elementary? Diet Mac OS, all I'm gonna say."},
      {"EndeavourOS", "Too scared of the terminal to use Arch?"},
      {"Endless OS", "Yet another atomic Debian distro."},
      {"EuroLinux",
       "Ah yes, a Eurodistro for your Eurocomputer to access the Euronet."},
      {"Exherbo", "We have Gentoo at home."},
      {"Fedora Linux", "A new version of Fedora has probably released in the "
                       "time it took that last command to load."},
      {"FreeBSD",
       "FreeBSD? How long did your install take to properly configure?"},
      {"Garuda Linux",
       "Garuda? I can smell the G Fuel and gamer gunk from here."},
      {"Gentoo", "All that time spent compiling, all those years lost"},
      {"GhostBSD", "GhostBSD? Too scared to learn the FreeBSD setup process?"},
      {"Kali GNU/Linux", "I'm sure you understand all of the Kali tools."},
      {"KaOS", "Oh, you reallyyyyy like KDE."},
      {"KDE neon", "You know there are other desktop environments out there?"},
      {"Linux Mint", "Ah yes,Mint, AKA Debian-based distro #1952"},
      {"LMDE",
       "LMDE? I'm sure it'll be the standard Mint version in a few decades."},
      {"Manjaro Linux",
       "Let's hope your distro updates their SSL certificates."},
      {"Manjaro-ARM", "Running Manjaro on ARM? What was your thought process?"},
      {"NixOS", "NixOS? Nothing more comforting than stable leadership of a "
                "project, right?"},
      {"Nobara Linux", "All hail GloriousEggroll!"},
      {"openSUSE Leap", "OpenSUSE-Sprung, RHEL für Deutsche"},
      {"openSUSE Tumbleweed", "OpenSUSE Tumbleweed, Fedora für Deutsche"},
      {"OpenWrt", "Why on earth would you run this shell on a network device?"},
      {"Parrot OS",
       "You're not one of those Kali skids, you're a Parrot skid."},
      {"PikaOS", "Yet another distro for radical gamers."},
      {"Pop!_OS", "I imagine you heard about Linux from LTT"},
      {"Puppy", "Puppy? Are you running this on a potato?"},
      {"Raspbian GNU/Linux", "We have Debian at home."},
      {"Red Hat Enterprise Linux", "Was the license cost worth it?"},
      {"Redox OS", "Redox? Tell me more about how great Rust is."},
      {"Rocky Linux", "Rocky? RHEL for human beings."},
      {"Slackware", "YOU CAN BE WHAT YOU WON'T."},
      {"SLED",
       "SLED? I hope you're not running this on an enterprise machine."},
      {"SLES",
       "SLES? I hope you're not running this on an enterprise machine."},
      {"SteamOS", "Are you really daily driving a Steam Deck?"},
      {"Tails", "The glowies have compromised this system."},
      {"TencentOS Server", "Fùzhì jiù huì sǐ."},
      {"TinyCore", "Are you running this on a potato?"},
      {"Trisquel GNU/Linux",
       "Trisquel? How much time did you spend finding compatible hardware?"},
      {"Ubuntu Kylin", "Kylin? You'd better be running the RISC-V version."},
      {"Ubuntu", "Ubuntu? Opt-out telemetry, proprietary app store, shopping "
                 "ads; why not run Windows at this point?"},
      {"Ultramarine Linux", "A new version of Fedora has probably released in "
                            "the time it took that last command to load."},
      {"Vanilla OS", "You really couldn't pick a single distro?"},
      {"Void", "Ah I see. An anime-watching, anti-normie, 4chan-browsing, "
               "musl-using, rust-fearing, Mental Outlaw-agreeing, "
               "Suckless-shilling, crypto-mining, Void user."},
      {"Zorin OS", "Zorin? Were the extra layout options worth the money?"}};
  joke_binary_search(known_distros, NUM_OF_KNOWN_DISTROS, os_release);
  os_release = os_release - 6;
  free(os_release);
}

void tease_kernel(void) {
  static int called = 0;
  if (called == 1) {
    return;
  }
  called = 1;

  struct utsname buffer;
  if (uname(&buffer) == -1) {
    error_msg("Failed to retrieve kernel information", true);
  }

  char *is_lts = strstr(buffer.release, "lts");
  if (is_lts) {
    printf("LTS? The future is now, old man.");
  }

  if (strstr(buffer.release, "zen")) {
    printf("You're on Zen? How's that placebo hitting for you?");
  }

  int kernel_big_num = atoi(&buffer.release[0]);
  if (kernel_big_num < 6 && !is_lts) {
    printf("Please tell me this thing isn't connected to the internet on such "
           "an old kernel.\n");
  }
}

void tease_program(const char *program_name) {
  static const struct joke known_programs[NUM_OF_KNOWN_PROGRAMS] = {
      {"audacity", "At least someone is listening to your recordings."},
      {"bleachbit", "Pray that your email server isn't being audited."},
      {"brasero", "What is this ancient technology?"},
      {"clamscan",
       "Signature-based detection will surely hold up against zero days."},
      {"code", "EEE, MICROSOFT BAD."},
      {"discord", "Make sure to enable application privacy before doing any "
                  "independent research."},
      {"ed", "It's almost time for your nap Grandpa."},
      {"emerge", "Does compiling make you feel more productive?"},
      {"firefox", "The ship is sinking my friend."},
      {"handbrake", "I hope this is all legitimately acquired media."},
      {"hashcat", ETHICAL_QUESTIONING_MSG},
      {"john", ETHICAL_QUESTIONING_MSG},
      {"lutris", "Lutris, a launcher for your launchers."},
      {"lynx",
       "Nice, you can browse the two websites that don't require Javascript."},
      {"mc", "The 90s were a long time ago."},
      {"nano", "Baby's first text editor."},
      {"ncmpcpp", "How long did it take to configure that?"},
      {"nix", "Make sure to tell everyone you know about Nix, they'll really "
              "appreciate it."},
      {"nmap", "Did your neighbors get into the network again?"},
      {"nvim", "Did Brodie or the Primeagen send you?"},
      {"pandoc",
       "Have those hours spent writing LaTeX made you any better at math?"},
      {"pcsx2", "Feel old yet?"},
      {"ranger", "Ah I see, commands are just too hard for you."},
      {"retroarch", "Ah yes, the systemd of emulators."},
      {"rofi", "Real Linux users only need fzf."},
      {"setoolkit", "What, are you trying to hack your Pre-Algebra teacher?"},
      {"snap", "Oh boy, everyone's favorite app packaging format."},
      {"sqlmap", "WHOA U R A 1337 HAXOR!"},
      {"steam", "Your To-Do list isn't getting any smaller"},
      {"thunar", "Are you too good for the CLI now?"},
      {"tmux", "Kitty panes better."},
      {"tor", "I have notified the FBI of your activities."},
      {"transmission", "Downloading more Linux ISOs?"},
      {"valgrind", "You couldn't just program it right the first time?"},
      {"virt-manager", "Real Linux chads write their QEMU scripts themselves."},
      {"vlc", "Do you really need all that functionality to watch Anime?"},
      {"waybar", "I'm sure all the modules you're using are deeply important "
                 "to have on-screen constantly."},
      {"wine", "How dare you run that heathen proprietary software."},
      {"wireshark",
       "Why bother? You don't understand any of the packet fields anyway."},
      {"zenmap", "Is the CLI too hard for you?"},
      {"zoom",
       "You might as well just livestream your webcam to the internet."}};

  joke_binary_search(known_programs, NUM_OF_KNOWN_PROGRAMS, program_name);
}

// This will likely be overhauled in the future, as many terminals use the same
// TERM value
void tease_terminal(void) {
  static int called = 0;
  if (called == 1) {
    return;
  }
  called = 1;
  const char *term = getenv("TERM");
  if (!term) {
    error_msg(env_fail_msg, false);
  }

  static const struct joke known_terminals[NUM_OF_KNOWN_TERMINALS] = {
      {"st-256color", "An st user? You must be fun at parties."},
      {"xterm-kitty", "AN UPDATE FOR KITTY IS AVAILABLE."}};

  for (int i = 0; i < NUM_OF_KNOWN_TERMINALS; i++) {
    if (strcmp(term, known_terminals[i].name) == 0) {
      printf("%s\n", known_terminals[i].comment);
      return;
    }
  }
}
