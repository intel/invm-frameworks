%define build_version 99.99.99.9999

Name:           invm-frameworks
Version:        %{build_version}
Release:        1%{?dist}
Summary:        Framework for Storage I18N, CLI and CIM applications
License:        BSD
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

%package -n %{name}-devel
Summary:        Development files for %{name}
License:        BSD
Group:          Development/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description -n %{name}-devel
The %{name}-devel package contains header files for
developing applications that use %{name}.

%package -n libinvm-i18n
Summary:        Internationalization library
License:        BSD
Group:          Development/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description -n libinvm-i18n
The libinvm-i18n package supports a subset of Internationalization (I18N)
functionality.

%package -n libinvm-i18n-devel
Summary:        Development files for libinvm-i18n
License:        BSD
Group:          Development/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description -n libinvm-i18n-devel
The libinvm-i18n-devel package contains header files for
developing applications that use libinvm-i18n.

%package -n libinvm-cli
Summary:        Framework for Storage CLI applications
License:        BSD
Group:          Development/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description -n libinvm-cli
The libinvm-cli package supports storage command line interface (CLI)
applications.

%package -n libinvm-cli-devel
Summary:        Development files for libinvm-cli
License:        BSD
Group:          Development/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description -n libinvm-cli-devel
The libinvm-cli-devel package contains header files for
developing applications that use libinvm-cli.

%package -n libinvm-cim
Summary:        Framework for Storage CIM providers
License:        BSD
Group:          Development/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description -n libinvm-cim
The libinvm-cim package supports storage common information model (CIM)
providers.

%package -n libinvm-cim-devel
Summary:        Development files for libinvm-cim
License:        BSD
Group:          Development/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description -n libinvm-cim-devel
The libinvm-cim-devel package contains header files for
developing applications that use libinvm-cim.

%prep
%setup -q -n %{name}-%{version}

%build
%cmake
make BUILDNUM=%{build_version} RELEASE=1 CFLAGS_EXTERNAL="%{?optflags}" %{?_smp_mflags}

%install
mkdir -p %{buildroot}%{_libdir}
cp -rf ./output/build/linux/debug/libinvm-i18n.so* %{buildroot}%{_libdir}
cp -rf ./output/build/linux/debug/libinvm-cim.so* %{buildroot}%{_libdir}
cp -rf ./output/build/linux/debug/libinvm-cli.so* %{buildroot}%{_libdir}
mkdir -p %{buildroot}%{_includedir}
cp -rf ./output/build/linux/debug/include/libinvm-i18n %{buildroot}%{_includedir}
cp -rf ./output/build/linux/debug/include/libinvm-cim %{buildroot}%{_includedir}
cp -rf ./output/build/linux/debug/include/libinvm-cli %{buildroot}%{_includedir}

%files
%doc README.md
%{_libdir}/libinvm-i18n.so.*
%{_libdir}/libinvm-cli.so.*
%{_libdir}/libinvm-cim.so.*
%license LICENSE

%files -n %{name}-devel
%doc README.md
%{_libdir}/libinvm-i18n.so
%{_includedir}/libinvm-i18n
%{_libdir}/libinvm-cli.so
%{_includedir}/libinvm-cli
%{_libdir}/libinvm-cim.so
%{_includedir}/libinvm-cim
%license LICENSE

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files -n libinvm-i18n
%doc README.md
%{_libdir}/libinvm-i18n.so.*
%license LICENSE

%files -n libinvm-i18n-devel
%doc README.md
%{_includedir}/libinvm-i18n
%license LICENSE

%post -n libinvm-i18n -p /sbin/ldconfig
%postun -n libinvm-i18n -p /sbin/ldconfig

%files -n libinvm-cli 
%doc README.md
%{_libdir}/libinvm-cli.so.*
%license LICENSE

%files -n libinvm-cli-devel
%doc README.md
%{_includedir}/libinvm-cli
%license LICENSE

%post -n libinvm-cli -p /sbin/ldconfig
%postun -n libinvm-cli -p /sbin/ldconfig

%files -n libinvm-cim 
%doc README.md
%{_libdir}/libinvm-cim.so.*
%license LICENSE

%files -n libinvm-cim-devel
%doc README.md
%{_includedir}/libinvm-cim
%license LICENSE

%post -n libinvm-cim -p /sbin/ldconfig
%postun -n libinvm-cim -p /sbin/ldconfig

%changelog
* Wed May 24 2017 Namratha Kothapalli <namratha.n.kothapalli@intel.com> - 01.00.00.2000-1
- Initial rpm release
