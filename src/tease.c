#include "tease.h"
#include "error.h"

int teasing_enabled = 1;

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
    fprintf(stderr, env_fail_msg, "XDG_CURRENT_DESKTOP");
    return;
  }
  // TO DO - Confirm XDG_CURRENT_DESKTOP values for each of these
  static const struct joke known_desktops[NUM_OF_KNOWN_DESKTOPS] = {
      {"2bwm", "PLACEHOLDER"},
      {"9wm", "PLACEHOLDER"},
      {"afterstep", "PLACEHOLDER"},
      {"awesome", "Dear Ritchie, you configure your window manager in LUA?"},
      {"berry", "PLACEHOLDER"},
      {"blackbox", "PLACEHOLDER"},
      {"bspwm", "PLACEHOLDER"},
      {"budgie", "PLACEHOLDER"},
      {"cde", "CDE? Can you help me fix my walkman?"},
      {"cinnamon",
       "Oh, you're on Cinnamon. How's your first week in linux going?"},
      {"compiz", "PLACEHOLDER"},
      {"COSMIC", "COSMIC and clowniSH? You must really like alpha software."},
      {"cutefish", "PLACEHOLDER"},
      {"cwm", "PLACEHOLDER"},
      {"deepin", "Deepin? Bold choice trusting them with your data."},
      {"dk", "PLACEHOLDER"},
      {"dwm", "DWM, ultra minimalism with a X11 dependency."},
      {"echinus", "PLACEHOLDER"},
      {"ede", "PLACEHOLDER"},
      {"eggwm", "PLACEHOLDER"},
      {"enlightenment",
       "Wow, Enlightenment? That's a name I haven't heard in a long time..."},
      {"evilwm", "PLACEHOLDER"},
      {"exwm", "PLACEHOLDER"},
      {"fluxbox", "PLACEHOLDER"},
      {"flwm", "PLACEHOLDER"},
      {"frankenwm", "FrankenWM, because DWM is just too mainstream."},
      {"fvwm", "PLACEHOLDER"},
      {"gala", "PLACEHOLDER"},
      {"GNOME-Flashback", "You know MATE exists, right?"},
      {"gnome", "How is the search for GNOME extensions that haven't been "
                "deprecated going?"},
      {"goomwm", "PLACEHOLDER"},
      {"hackmatrix", "Oh my, a 3D window manager? That must make you so much more productive."},
      {"herbstluftwm", "PLACEHOLDER"},
      {"hyprland", "Are the fancy animations worth the instability?"},
      {"i3", "I'm sure that manual tiling saves you so much time."},
      {"icewm", "I'm sure that manual tiling saves you so much time."},
      {"jbwm", "PLACEHOLDER"},
      {"jwm", "PLACEHOLDER"},
      {"karmen", "PLACEHOLDER"},
      {"kde", "A KDE user? I'm sure you'll need access to all those options."},
      {"larswm", "PLACEHOLDER"},
      {"leftwm", "PLACEHOLDER"},
      {"liri", "PLACEHOLDER"},
      {"lumina", "PLACEHOLDER"},
      {"lwm", "PLACEHOLDER"},
      {"lxde", "PLACEHOLDER"},
      {"lxqt", "PLACEHOLDER"},
      {"MATE", "MATE? The 2000s were pretty great, weren't they?"},
      {"maui", "PLACEHOLDER"},
      {"maxxdesktop", "PLACEHOLDER"},
      {"moksha", "PLACEHOLDER"},
      {"niri", "PLACEHOLDER"},
      {"notion", "PLACEHOLDER"},
      {"openbox", "Did you know that some floating WMs actually let you snap windows with your mouse?"},
      {"orbitiny", "PLACEHOLDER"},
      {"pantheon", "PLACEHOLDER"},
      {"paperde", "PLACEHOLDER"},
      {"pawm", "PLACEHOLDER"},
      {"pekwm", "PLACEHOLDER"},
      {"phosh", "PLACEHOLDER"},
      {"plainDE", "PLACEHOLDER"},
      {"qtile", "PLACEHOLDER"},
      {"ratpoison", "Go on, tell me all about how your WM helps you avoid RSI. I care immensely."},
      {"sawfish", "PLACEHOLDER"},
      {"sowm", "PLACEHOLDER"},
      {"spectrwm", "You can probably spare more than 20MB of memory for your WM."},
      {"stumpwm", "PLACEHOLDER"},
      {"sugar", "PLACEHOLDER"},
      {"sway", "You just can't let go of i3, can you?"}, // CONFIRMED
      {"tde", "PLACEHOLDER"},
      {"theDesk", "PLACEHOLDER"},
      {"tinywm", "PLACEHOLDER"},
      {"trinity", "PLACEHOLDER"},
      {"ukui", "PLACEHOLDER"},
      {"Unity", "Unity? It's okay, you can move on."},
      {"windowmaker", "PLACEHOLDER"},
      {"wingo", "PLACEHOLDER"},
      {"worm", "PLACEHOLDER"},
      {"xfce", "XFCE? Booooring"},
      {"xmonad", "PLACEHOLDER"},
      {"zwm", "PLACEHOLDER"}};
  joke_binary_search(known_desktops, NUM_OF_KNOWN_DESKTOPS, desktop);
}

void tease_kernel(void) {
  static int called = 0;
  if (called == 1) {
    return;
  }
  called = 1;

  struct utsname buffer;
  if (uname(&buffer) == -1) {
    perror("uname");
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
    fprintf(stderr, env_fail_msg, term);
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

int program_is_blacklisted(const char *program_name) {
  static const struct joke blacklisted_programs[NUM_OF_BLACKLISTED_PROGRAMS] = {
      {"emacs", "No, use vim."},
      {"neofetch", "Do we really need another minimalist cyberpunk anime "
                   "girl tiling WM rice?"},
      {"surf", "Use a real web browser."}};

  for (int i = 0; i < NUM_OF_BLACKLISTED_PROGRAMS; i++) {
    if (strcmp(program_name, blacklisted_programs[i].name) == 0) {
      printf("%s\n", blacklisted_programs[i].comment);
      return 1;
    }
  }
  return 0;
}

void tease_program(const char *program_name) {
  static const struct joke known_programs[NUM_OF_KNOWN_PROGRAMS] = {
      {"7z", "PLACEHOLDER"},
      {"aircrack-ng", "PLACEHOLDER"},
      {"aireplay-ng", "PLACEHOLDER"},
      {"ansible", "PLACEHOLDER"},
      {"apt", "PLACEHOLDER"},
      {"auditd", "PLACEHOLDER"},
      {"audacity", "PLACEHOLDER"},
      {"bleachbit", "Pray that your email server isn't being audited."},
      {"blender", "PLACEHOLDER"},
      {"brasero", "PLACEHOLDER"},
      {"cargo", "PLACEHOLDER"},
      {"chkrootkit", "PLACEHOLDER"},
      {"clamscan", "PLACEHOLDER"},
      {"clang", "PLACEHOLDER"},
      {"cmake", "PLACEHOLDER"},
      {"cmus", "PLACEHOLDER"},
      {"code", "EEE, MICROSOFT BAD."},
      {"conky", "PLACEHOLDER"},
      {"cron", "PLACEHOLDER"},
      {"curl", "PLACEHOLDER"},
      {"discord", "Make sure to enable application privacy before doing any independent research."},
      {"dmenu", "PLACEHOLDER"},
      {"docker", "PLACEHOLDER"},
      {"ed", "PLACEHOLDER"},
      {"emerge", "PLACEHOLDER"},
      {"ettercap", "PLACEHOLDER"},
      {"ffmpeg", "PLACEHOLDER"},
      {"firefox", "The ship is sinking my friend."},
      {"flatpak", "PLACEHOLDER"},
      {"fparted", "PLACEHOLDER"},
      {"gcc", "PLACEHOLDER"},
      {"gdb", "PLACEHOLDER"},
      {"gimp", "PLACEHOLDER"},
      {"git", "PLACEHOLDER"},
      {"gnuplot", "PLACEHOLDER"},
      {"gparted", "PLACEHOLDER"},
      {"handbrake", "I hope this is all legitimately acquired media."},
      {"hashcat", "PLACEHOLDER"},
      {"homebrew", "PLACEHOLDER"},
      {"htop", "PLACEHOLDER"},
      {"inkscape", "PLACEHOLDER"},
      {"john", "PLACEHOLDER"},
      {"jq", "PLACEHOLDER"},
      {"krita", "PLACEHOLDER"},
      {"lsof", "PLACEHOLDER"},
      {"lutris", "PLACEHOLDER"},
      {"lynx", "PLACEHOLDER"},
      {"make", "PLACEHOLDER"},
      {"man", "PLACEHOLDER"},
      {"man", "PLACEHOLDER"},
      {"mc", "PLACEHOLDER"},
      {"meson", "PLACEHOLDER"},
      {"nano", "PLACEHOLDER"},
      {"ncmpcpp", "PLACEHOLDER"},
      {"netcat", "PLACEHOLDER"},
      {"nikto", "PLACEHOLDER"},
      {"nix", "PLACEHOLDER"},
      {"nmap", "PLACEHOLDER"},
      {"nvim", "Did Brodie or the Primeagen send you?"},
      {"obs", "PLACEHOLDER"},
      {"pacman", "PLACEHOLDER"},
      {"pandoc", "PLACEHOLDER"},
      {"parted", "PLACEHOLDER"},
      {"paru", "PLACEHOLDER"},
      {"pcsx2", "PLACEHOLDER"},
      {"picom", "PLACEHOLDER"},
      {"pip", "PLACEHOLDER"},
      {"polybar", "PLACEHOLDER"},
      {"ranger", "Ah I see, commands are just too hard for you."},
      {"retroarch", "PLACEHOLDER"},
      {"rkhunter", "PLACEHOLDER"},
      {"rofi", "PLACEHOLDER"},
      {"setoolkit", "PLACEHOLDER"},
      {"snap", "PLACEHOLDER"},
      {"snort", "PLACEHOLDER"},
      {"sqlmap", "PLACEHOLDER"},
      {"steam", "Your To-Do list isn't getting any smaller"},
      {"strace", "PLACEHOLDER"},
      {"tar", "PLACEHOLDER"},
      {"testdisk", "PLACEHOLDER"},
      {"thunar", "Are you too good for the CLI now?"},
      {"tmux", "PLACEHOLDER"},
      {"tor", "PLACEHOLDER"},
      {"transmission", "PLACEHOLDER"},
      {"tshark", "PLACEHOLDER"},
      {"vagrant", "PLACEHOLDER"},
      {"valgrind", "PLACEHOLDER"},
      {"virt-manager", "PLACEHOLDER"},
      {"vlc", "PLACEHOLDER"},
      {"waybar", "PLACEHOLDER"},
      {"wget", "PLACEHOLDER"},
      {"wine", "PLACEHOLDER"},
      {"wireshark", 
       "Why bother? You don't understand any of the packet fields anyway."},
      {"zenmap", "PLACEHOLDER"},
      {"zoom", "PLACEHOLDER"}};

  joke_binary_search(known_programs, NUM_OF_KNOWN_PROGRAMS, program_name);
}
