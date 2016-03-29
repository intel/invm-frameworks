%define rpm_name intelcliframework
%define build_version 99.99.99.9999
%define build_release 1
%define dname %{rpm_name}-devel

Name:           %{rpm_name}-libs
Version:		%{build_version}
Release:		%{build_release}%{?dist}
Summary:		Framework for Intel Storage CLI Binaries
License:        BSD
Group:          Development/Libraries
URL:			http://www.intel.com
Source:         %{rpm_name}.tar.bz2

%define  debug_package %{nil}

%description
Framework libraries for the Intel Storage CLI binaries

%package -n %dname
Summary:        Development files for %{name}
License:        BSD
Group:          Development/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description -n %dname
The %{name}-devel package contains header files for
developing applications that use %{name}.

%prep
%setup -q -n %{rpm_name}

%build
make BUILDNUM=%{build_version} RELEASE=1

%install
make install RELEASE=1 RPM_ROOT=%{buildroot} LIB_DIR=%{_libdir} INCLUDE_DIR=%{_includedir}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(755,root,root,755)
%{_libdir}/libcliframework.so.*
%license LICENSE

%files -n %dname
%defattr(755,root,root,755)
%{_libdir}/libcliframework.so
%dir %{_includedir}/intel_cli_framework
%attr(644,root,root) %{_includedir}/intel_cli_framework/*.h
%license LICENSE

%changelog
* Wed Dec 24 2015 nicholas.w.moulin@intel.com
- Initial rpm release
