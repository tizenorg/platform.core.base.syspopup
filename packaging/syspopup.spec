Name:       syspopup
Summary:    syspopup package
Version:	0.0.67
Release:    1
Group:      TO_BE/FILLED_IN
License:    Apache-2.0
Source0:    syspopup-%{version}.tar.gz
Requires(post): /sbin/ldconfig
Requires(post): /bin/touch
Requires(postun): /sbin/ldconfig
BuildRequires:  cmake
BuildRequires:  pkgconfig(sqlite3)
BuildRequires:  pkgconfig(bundle)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(utilX)
BuildRequires:  pkgconfig(dbus-glib-1)
BuildRequires:  pkgconfig(x11)
BuildRequires:  pkgconfig(heynoti)
BuildRequires:  pkgconfig(aul)
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(appcore-efl)


%description
syspopup package for popup


%package devel
Summary:    syspopup development package
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
syspopup development package popup

%package caller
Summary:    syspopup-caller package
Group:      TO_BE/FILLED_IN
Requires:   %{name} = %{version}-%{release}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description caller
syspopup-caller package for popup

%package caller-devel
Summary:    syspopup-caller development package
Group:      TO_BE/FILLED_IN
Requires:   %{name}-devel = %{version}-%{release}

%description caller-devel
syspopup-caller development package for popup


%prep
%setup -q -n %{name}-%{version}


%build
CFLAGS=${_cflags} cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DEXTRA_CFLAGS=-fPIC
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

touch %{buildroot}%{_datadir}/popup_noti_term



%post
/sbin/ldconfig

mkdir -p /opt/dbspace/
sqlite3 /opt/dbspace/.syspopup.db < /opt/share/syspopup_db.sql
rm -rf /opt/share/syspopup_db.sql
chown root:5000 /opt/dbspace/.syspopup.db
chown root:5000 /opt/dbspace/.syspopup.db-journal
chmod 664 /opt/dbspace/.syspopup.db
chmod 664 /opt/dbspace/.syspopup.db-journal


%postun -p /sbin/ldconfig




%post caller -p /sbin/ldconfig

%postun caller -p /sbin/ldconfig




%files
%{_datadir}/icons/default/small/org.tizen.syspopup-app.png
%{_bindir}/sp_test
%{_bindir}/syspopup-app
%{_libdir}/libsyspopup.so.0.1.0
/opt/share/syspopup_db.sql
/opt/share/applications/org.tizen.syspopup-app.desktop
%{_datadir}/popup_noti_term


%files devel
%{_includedir}/SLP_SYSPOPUP_PG.h
%{_includedir}/syspopup.h
%{_libdir}/libsyspopup.so
%{_libdir}/pkgconfig/syspopup.pc

%files caller
%{_libdir}/libsyspopup_caller.so.0.1.0

%files caller-devel
%{_libdir}/libsyspopup_caller.so
%{_includedir}/syspopup_caller.h
%{_libdir}/pkgconfig/syspopup-caller.pc

