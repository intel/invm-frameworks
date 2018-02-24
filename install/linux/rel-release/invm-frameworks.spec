%define build_version 99.99.99.9999

Name:           invm-frameworks
Version:        %{build_version}
Release:        1%{?dist}
Summary:        Framework for Storage I18N, CLI and CIM applications
License:        BSD-3-Clause
Group:          Development/Libraries
URL:            https://01.org/intel-nvm-frameworks
Source:         https://github.com/01org/invm-frameworks/archive/v%{version}.tar.gz#/%{name}-%{version}.tar.gz
BuildRequires:  gettext
BuildRequires:  cmake
ExclusiveArch:  x86_64

%description
Framework library supporting a subset of Internationalization (I18N)
functionality, storage command line interface (CLI) applications, storage
common information model (CIM) providers.

%package -n libinvm-i18n
Summary:        Internationalization library
License:        BSD-3-Clause
Group:          Development/Libraries
Obsoletes:      invm-frameworks

%description -n libinvm-i18n
The libinvm-i18n package supports a subset of Internationalization (I18N)
functionality.

%package -n libinvm-i18n-devel
Summary:        Development files for libinvm-i18n
License:        BSD-3-Clause
Group:          Development/Libraries
Requires:       libinvm-i18n = %{version}-%{release}
Obsoletes:      invm-frameworks-devel

%description -n libinvm-i18n-devel
The libinvm-i18n-devel package contains header files for
developing applications that use libinvm-i18n.

%package -n libinvm-cli
Summary:        Framework for Storage CLI applications
License:        BSD-3-Clause
Group:          Development/Libraries
Obsoletes:      invm-frameworks

%description -n libinvm-cli
The libinvm-cli package supports storage command line interface (CLI)
applications.

%package -n libinvm-cli-devel
Summary:        Development files for libinvm-cli
License:        BSD-3-Clause
Group:          Development/Libraries
Requires:       libinvm-cli = %{version}-%{release}
Obsoletes:      invm-frameworks-devel

%description -n libinvm-cli-devel
The libinvm-cli-devel package contains header files for
developing applications that use libinvm-cli.

%package -n libinvm-cim
Summary:        Framework for Storage CIM providers
License:        BSD-3-Clause
Group:          Development/Libraries
Obsoletes:      invm-frameworks

%description -n libinvm-cim
The libinvm-cim package supports storage common information model (CIM)
providers.

%package -n libinvm-cim-devel
Summary:        Development files for libinvm-cim
License:        BSD-3-Clause
Group:          Development/Libraries
Requires:       libinvm-cim = %{version}-%{release}
Obsoletes:      invm-frameworks-devel

%description -n libinvm-cim-devel
The libinvm-cim-devel package contains header files for
developing applications that use libinvm-cim.

%prep
%setup -q -n %{name}-%{version}

%build
%cmake -DBUILDNUM=%{version} -DCMAKE_INSTALL_PREFIX=/usr -DRELEASE=ON -DRPM_BUILD=ON \
  -DLINUX_PRODUCT_NAME=%{name} -DCMAKE_INSTALL_LIBDIR=%{_libdir} \
  -DCMAKE_INSTALL_INCLUDEDIR=%{_includedir} -DCFLAGS_EXTERNAL="%{?optflags}" \
  -DEXTERNAL=ON
make -f Makefile %{?_smp_mflags}

%install
%{!?_cmake_version: cd build}
make -f Makefile install DESTDIR=%{buildroot}

%files -n libinvm-i18n
%defattr(-,root,root)
%doc README.md
%{_libdir}/libinvm-i18n.so.*
%license LICENSE

%files -n libinvm-i18n-devel
%defattr(-,root,root)
%doc README.md
%{_libdir}/libinvm-i18n.so
%{_includedir}/libinvm-i18n
%{_libdir}/pkgconfig/libinvm-i18n.pc
%license LICENSE

%post -n libinvm-i18n -p /sbin/ldconfig
%postun -n libinvm-i18n -p /sbin/ldconfig

%files -n libinvm-cli
%defattr(-,root,root)
%doc README.md
%{_libdir}/libinvm-cli.so.*
%license LICENSE

%files -n libinvm-cli-devel
%defattr(-,root,root)
%doc README.md
%{_libdir}/libinvm-cli.so
%{_includedir}/libinvm-cli
%{_libdir}/pkgconfig/libinvm-cli.pc
%license LICENSE

%post -n libinvm-cli -p /sbin/ldconfig
%postun -n libinvm-cli -p /sbin/ldconfig

%files -n libinvm-cim
%defattr(-,root,root)
%doc README.md
%{_libdir}/libinvm-cim.so.*
%license LICENSE

%files -n libinvm-cim-devel
%defattr(-,root,root)
%doc README.md
%defattr(-,root,root)
%{_libdir}/libinvm-cim.so
%{_includedir}/libinvm-cim
%{_libdir}/pkgconfig/libinvm-cim.pc
%license LICENSE

%post -n libinvm-cim -p /sbin/ldconfig
%postun -n libinvm-cim -p /sbin/ldconfig

%changelog
