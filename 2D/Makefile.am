
AM_CXXFLAGS = -I../libvb

if DISPLAY_SDL
AM_CXXFLAGS += @SDL_CFLAGS@
AM_LDFLAGS = @SDL_LIBS@
else
if !DISPLAY_NOX
AM_CXXFLAGS += @X_CFLAGS@
AM_LDFLAGS = @X_LIBS@ -lX11
endif
endif

LDADD = ../libvb/libvb.la


bin_PROGRAMS = Contact
Contact_SOURCES = Contact.cpp

bin_PROGRAMS += Contact-hex
Contact_hex_SOURCES = Contact-hex.cpp

bin_PROGRAMS += DLA
DLA_SOURCES = DLA.cpp

bin_PROGRAMS += Ising
Ising_SOURCES = Ising.cpp

bin_PROGRAMS += LERW
LERW_SOURCES = LERW.cpp

bin_PROGRAMS += MB
MB_SOURCES = MB.cpp

bin_PROGRAMS += SAW
SAW_SOURCES = SAW.cpp

bin_PROGRAMS += DDLA
DDLA_SOURCES = DDLA.cpp

bin_PROGRAMS += Perco-bond
Perco_bond_SOURCES = Perco-bond.cpp

bin_PROGRAMS += Perco-grad
Perco_grad_SOURCES = Perco-grad.cpp

bin_PROGRAMS += Perco-site
Perco_site_SOURCES = Perco-site.cpp

bin_PROGRAMS += SLE
SLE_SOURCES = SLE.cpp

bin_PROGRAMS += Perco-bond-asym
Perco_bond_asym_SOURCES = Perco-bond-asym.cpp

bin_PROGRAMS += Coupling-perco
Coupling_perco_SOURCES = Coupling-perco.cpp

bin_PROGRAMS += Ising-cftp
Ising_cftp_SOURCES = Ising-cftp.cpp

bin_PROGRAMS += Once
Once_SOURCES = Once.cpp

bin_PROGRAMS += MDLA
MDLA_SOURCES = MDLA.cpp

bin_PROGRAMS += Perco-asym
Perco_asym_SOURCES = Perco-asym.cpp

bin_PROGRAMS += Glass
Glass_SOURCES = Glass.cpp

bin_PROGRAMS += FPP
FPP_SOURCES = FPP.cpp

bin_PROGRAMS += Pivot
Pivot_SOURCES = Pivot.cpp

bin_PROGRAMS += Perco-oriented
Perco_oriented_SOURCES = Perco-oriented.cpp

bin_PROGRAMS += RPNG
RPNG_SOURCES = RPNG.cpp

bin_PROGRAMS += Gacs5
Gacs5_SOURCES = Gacs5.cpp

bin_PROGRAMS += LPP
LPP_SOURCES = LPP.cpp
