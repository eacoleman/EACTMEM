C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
C
      Subroutine TopWidth(tmass,wmass,alphas,twidth)
C
C     on shell width of top quark including QCD corrections, c.f.
C     M.Jezabek and J.H. Kuhn, Nucl. Phys. B314(1989)1
C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
C
      Implicit real*8(a-h,o-z)
      External ddilog
      data gfermi/1.16639d-5/
      data pi/3.141592653589793238d0/
C
C---------------------------------------------------------------------
C
      f(x)= pi**2+2*ddilog(x)-2*ddilog(1-x)+(4*x*(1-x-2*x**2)*dlog(x)+
     $2*(1-x)**2*(5+4*x)*dlog(1-x)-(1-x)*(5+9*x-6*x**2) ) /
     $(2*(1-x)**2*(1+2*x))
      y= (wmass/tmass)**2
      twidth= gfermi*tmass**3/(8*sqrt(2d0)*pi)*(1-y)**2*(1+2*y)*
     $(1- 2d0/3*alphas/pi*f(y))
      end
C
C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
