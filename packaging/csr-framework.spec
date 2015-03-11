Summary: A general purpose content screening and reputation solution
Name: csr-framework
Version: 1.1.0
Release: 2

Source: %{name}-%{version}.tar.gz

License: BSD
Group: System/Libraries
URL: http://tizen.org

BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)

%description
csr-framework

%package devel
Summary:    Development files for csr-framework
Group:      TO_BE/FILLED_IN
Requires:   %{name} = %{version}

%description devel
To be described


%package test
Summary:    test program for csr-framework
Group:      TO_BE/FILLED_IN
Requires:   %{name} = %{version}

%description test
Comaptilibty test program


%prep
%setup -q 


%build
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}

mkdir -p %{buildroot}/usr/share/license
cp -f LICENSE %{buildroot}/usr/share/license/%{name}

%make_install


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%defattr(-,root,root,-)
%{_libdir}/libsecfw.so
/usr/share/license/%{name}

%files devel
%defattr(-,root,root,-)
%doc README
%doc doc/
%{_includedir}/TCSErrorCodes.h
%{_includedir}/TCSImpl.h
%{_includedir}/TWPImpl.h
%{_libdir}/pkgconfig/*.pc

%files test
%defattr(-,root,root,-)
/usr/sbin/*

