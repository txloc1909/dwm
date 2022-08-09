/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 4;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 6;        /* horiz inner gap between windows */
static const unsigned int gappiv    = 6;        /* vert inner gap between windows */
static const unsigned int gappoh    = 6;        /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 6;        /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int user_bh            = 24;       /* 0 means that dwm will calculate bar height, >= 1 means dwm will user_bh as bar height */
#define ICONSIZE (bh - 6)                       /* icon size */
#define ICONSPACING 6                           /* space between icon and title */
static const int focusonwheel       = 0;
static const char *fonts[]          = {
    "Source Code Pro for Powerline:size=10",
    "Ubuntu Mono:size=12",
    "NotoColorEmoji:size=10:antialias=true:autohint=true",
    "NotoEmoji:size=10",
    "monospace:size=9",
};
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_blue[]        = "#034685";
static const char col_green[]       = "#a6e22e";
static const char *colors[][3]      = {
    /*               fg         bg         border   */
    [SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
    [SchemeSel]  = { col_gray4, col_cyan,  col_cyan },
};

static const XPoint stickyicon[]    = { {0,0}, {4,0}, {4,8}, {2,6}, {0,8}, {0,0} }; /* represents the icon as an array of vertices */
static const XPoint stickyiconbb    = {4,8};	/* defines the bottom right corner of the polygon's bounding box (speeds up scaling) */

static const char *const autostart[] = {
    "slstatus", NULL,
    "nm-applet", NULL,
    "volumeicon", NULL,
    "nitrogen", "--restore", NULL,
    "sxhkd", NULL,
    "picom", NULL,
    "dunst", NULL,
    "/usr/bin/gnome-keyring-daemon", "--start", NULL,
    NULL /* terminate */
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
    /* xprop(1):
     *  WM_CLASS(STRING) = instance, class
     *  WM_NAME(STRING) = title
     */
    /* class      instance    title       tags mask     isfloating   monitor */
    { "firefox",  NULL,       NULL,       1,            0,           -1 },
    { "Zathura",  "org.pwmt.zathura", NULL, 1 << 2,     0,           -1 },
    { "notion-snap", "notion-snap", NULL, 1 << 3,       0,           -1 },
    { "Brave-browser", "youtube.com", NULL, 1 << 7,     0,           -1 },
    { "Spotify",  "spotify",  "Spotify",  1 << 7,       0,           -1 },
    { "Brave-browser", "brave-browser", NULL, 1 << 8,   0,           -1 },
    { "Ferdi",    "ferdi",    NULL,       1 << 8,       0,           -1 },

    //{ NULL,       NULL,       NULL,       0,            0,           -1 },

};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"


static const Layout layouts[] = {
    /* symbol     arrange function */
    { "[]=",      tile },    /* first entry is default */
    { "><>",      NULL },    /* no layout function means floating behavior */
    { "[M]",      monocle },
    { "[D]",      deck },
    { "|M|",      centeredmaster },
    { ">M>",      centeredfloatingmaster },
};

/* key definitions */
#define MODKEY Mod4Mask
#define ALTKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
    { MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
    { MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
    { MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
    { MODKEY|ALTKEY,                KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
//static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *dmenucmd[] = { "dmenu_run", NULL };
static const char *termcmd[]  = { "alacritty", NULL };

#include "movestack.c"
static Key keys[] = {
    /* modifier                     key        function        argument */

    /* Spawning program                                                 */
    /* Already defined in sxhkd, lists here to avoid overriding
    { MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
    { MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
    { MODKEY,                       XK_w,      spawn,          SHCMD("$BROWSER") },
    { MODKEY,                       XK_e,      spawn,          SHCMD("nemo") },
    { MODKEY,                       XK_r,      spawn,          SHCMD("rofi -show drun -monitor -1") },
    { MODKEY,                       XK_y,      spawn,          SHCMD("youtube") },
    */

    /* Appearance                                                       */
    { MODKEY,                       XK_b,      togglebar,      {0} },
    { MODKEY,                       XK_a,      defaultgaps,    {0} },
    { MODKEY|ShiftMask,             XK_a,      togglegaps,     {0} },
    { MODKEY|ShiftMask,             XK_z,      incrgaps,       {.i = -2} },
    { MODKEY|ShiftMask,             XK_x,      incrgaps,       {.i = +2} },


    /* Window manipulation                                              */
    { MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
    { MODKEY,                       XK_k,      focusstack,     {.i = -1 } },

    { MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
    { MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },

    { MODKEY|ShiftMask,             XK_h,      setcfact,       {.f = +0.25} },
    { MODKEY|ShiftMask,             XK_l,      setcfact,       {.f = -0.25} },
    { MODKEY|ShiftMask,             XK_n,      setcfact,       {.f =  0.00} },          // reset cfacts of all slave windows

    { MODKEY|ShiftMask,             XK_j,      movestack,      {.i = +1 } },
    { MODKEY|ShiftMask,             XK_k,      movestack,      {.i = -1 } },

    { MODKEY|ShiftMask,             XK_equal,  incnmaster,     {.i = +1 } },            // increase number of masters
    { MODKEY|ShiftMask,             XK_minus,  incnmaster,     {.i = -1 } },            // decrease number of masters

    { MODKEY,                       XK_f,      togglefullscr,  {0} },
    { MODKEY,                       XK_p,      togglefloating, {0} },
    { MODKEY,                       XK_s,      togglesticky,   {0} },
    { MODKEY,                       XK_u,      zoom,           {0} },                   // promote current window as master
    { MODKEY,                       XK_i,      switchcol,      {0} },
    { ALTKEY,                       XK_Tab,    swapfocus,      {0} },                   // toggle between 2 recent windows
    { MODKEY,                       XK_q,      killclient,     {0} },

    /* Layouts                                                          */
    { MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },    // tile
    { MODKEY|ShiftMask,             XK_f,      setlayout,      {.v = &layouts[1]} },    // floating
    { MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },    // monocle
    { MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[3]} },    // deck
    { MODKEY|ShiftMask,             XK_c,      setlayout,      {.v = &layouts[4]} },    // centermaster
    { MODKEY,                       XK_c,      setlayout,      {.v = &layouts[5]} },    // centeredfloatingmaster
    { MODKEY,                       XK_Tab,    setlayout,      {0} },                   // toggle between 2 recent layouts

    /* Tags                                                             */
    { ALTKEY,                       XK_space,  view,           {0} },                   // toggle between 2 recent tagsets
    { MODKEY,                       XK_0,      view,           {.ui = ~0 } },
    { MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
    TAGKEYS(                        XK_1,                      0)
    TAGKEYS(                        XK_2,                      1)
    TAGKEYS(                        XK_3,                      2)
    TAGKEYS(                        XK_4,                      3)
    TAGKEYS(                        XK_5,                      4)
    TAGKEYS(                        XK_6,                      5)
    TAGKEYS(                        XK_7,                      6)
    TAGKEYS(                        XK_8,                      7)
    TAGKEYS(                        XK_9,                      8)

    /* Monitors                                                         */
    { MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
    { MODKEY,                       XK_period, focusmon,       {.i = +1 } },
    { MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
    { MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },

    /* Quit & restart                                                   */
    { MODKEY|ShiftMask,             XK_q,      quit,           {0} },                   // quit dwm (logout)
    { MODKEY|ShiftMask,             XK_r,      quit,           {1} },                   // restart dwm
    { ControlMask|ALTKEY,           XK_Delete, spawn,          SHCMD("sysact"), },
    { MODKEY|ALTKEY,                XK_l,      spawn,          SHCMD("slock"), }        // lock screen
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
    /* click                event mask      button          function        argument */
    { ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
    { ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[0]} },

    { ClkWinTitle,          0,              Button2,        zoom,           {0} },

    { ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },

    /* windows                                                                       */
    { ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
    { ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
    { ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },

    /* tags                                                                          */
    { ClkTagBar,            0,              Button1,        view,           {0} },
    { ClkTagBar,            0,              Button3,        toggleview,     {0} },
    { ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
    { ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },

    /* adjust windows gaps                                                           */
    { ClkClientWin,         MODKEY,         Button4,        incrgaps,       {.i = +1} },
    { ClkClientWin,         MODKEY,         Button5,        incrgaps,       {.i = -1} },
};
