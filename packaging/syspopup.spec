%bcond_with x
%bcond_with wayland

Name:           syspopup
Summary:        Syspopup package
Version:        0.0.100
Release:        0
Group:          System/Libraries
License:        Apache-2.0
Source0:        syspopup-%{version}.tar.gz
Source1001:     %{name}.manifest
Source1002:     %{name}-devel.manifest
Source1003:     %{name}-caller.manifest
Source1004:     %{name}-caller-devel.manifest
Source1005:	org.tizen.aul.syspopup.conf.in

BuildRequires:  cmake
BuildRequires:  pkgconfig(sqlite3)
BuildRequires:  pkgconfig(bundle)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gio-2.0)
BuildRequires:	pkgconfig(elementary)
%if %{with x}
BuildRequires:  pkgconfig(utilX)
BuildRequires:  pkgconfig(x11)
BuildRequires:  pkgconfig(ecore-x)
%endif
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(libtzplatform-config)

%description
syspopup package for popup

%package devel
Summary:    Syspopup development package
Group:      System/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
syspopup development package popup

%package caller
Summary:          Syspopup-caller package
Group:            System/Libraries
Requires:         %{name} = %{version}-%{release}
Requires(post):   /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description caller
syspopup-caller package for popup

%package caller-devel
Summary:    Syspopup-caller development package
Group:      System/Development
Requires:   %{name} = %{version}-%{release}

%description caller-devel
syspopup-caller development package for popup

%prep
%setup -q
sed -i %{SOURCE1001} -e "s|TZ_SYS_DB|%TZ_SYS_DB|g"
cp %{SOURCE1001} %{SOURCE1002} %{SOURCE1003} %{SOURCE1004} .

%build
%cmake . \
%if %{with wayland} && !%{with x}
-Dwith_wayland=TRUE \
%endif
-DEXTRA_CFLAGS=-fPIC

make %{?jobs:-j%jobs}

%install
%make_install
mkdir -p %{buildroot}/usr/share/license
install LICENSE %{buildroot}/usr/share/license/%{name}
install LICENSE %{buildroot}/usr/share/license/%{name}-devel
install LICENSE %{buildroot}/usr/share/license/%{name}-caller
install LICENSE %{buildroot}/usr/share/license/%{name}-caller-devel

mkdir -p %{buildroot}%{TZ_SYS_DB}
sqlite3 %{buildroot}%{TZ_SYS_DB}/.syspopup.db < %{buildroot}/usr/share/syspopup/syspopup_db.sql
rm -rf %{buildroot}/usr/share/syspopup/syspopup_db.sql

touch %{buildroot}%{_datadir}/popup_noti_term

mkdir -p %{buildroot}%{_sysconfdir}/dbus-1/system.d
install -m 0644 %SOURCE1005 %{buildroot}%{_sysconfdir}/dbus-1/system.d/org.tizen.aul.syspopup.conf

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%post caller -p /sbin/ldconfig

%postun caller -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_datadir}/icons/default/small/org.tizen.syspopup-app.png
%{_bindir}/sp_test
%{_bindir}/syspopup-app
%{_libdir}/libsyspopup.so.0.1.0
%TZ_SYS_RO_PACKAGES/org.tizen.syspopup-app.xml
%{_datadir}/popup_noti_term

%attr(644,root,%{TZ_SYS_USER_GROUP}) %{TZ_SYS_DB}/.syspopup.db
%attr(644,root,%{TZ_SYS_USER_GROUP}) %{TZ_SYS_DB}/.syspopup.db-journal

/usr/share/license/%{name}

%files devel
%manifest %{name}-devel.manifest
%defattr(-,root,root,-)
%{_includedir}/SLP_SYSPOPUP_PG.h
%{_includedir}/syspopup.h
%{_libdir}/libsyspopup.so
%{_libdir}/pkgconfig/syspopup.pc
/usr/share/license/%{name}-devel

%files caller
%manifest %{name}-caller.manifest
%defattr(-,root,root,-)
%{_libdir}/libsyspopup_caller.so.0.1.0
/usr/share/license/%{name}-caller
%{_sysconfdir}/dbus-1/system.d/org.tizen.aul.syspopup.conf

%files caller-devel
%manifest %{name}-caller-devel.manifest
%defattr(-,root,root,-)
%{_libdir}/libsyspopup_caller.so
%{_includedir}/syspopup_caller.h
%{_libdir}/pkgconfig/syspopup-caller.pc
/usr/share/license/%{name}-caller-devel


%changelog
* Tue Aug 13 2013 - Hyungdeuk Kim <hd3.kim@samsung.com>
- Add key handler for esc of keyboard

* Fri Jun 21 2013 - Hyungdeuk Kim <hd3.kim@samsung.com>
- Change end key term info of wifi-qs

* Fri Apr 12 2013 - Hyungdeuk Kim <hd3.kim@samsung.com>
- Remove drm syspopup requested by hakjoo.ko

* Tue Apr 2 2013 - Hyugdeuk Kim <hd3.kim@samsung.com>
- Add syspopup_reset_timeout api
