C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
C
      Subroutine MYTOPWIDTH(mt,mw,wwidth,twidth)
C
C     on shell width of top quark including QCD corrections, c.f.
C     M.Jezabek and J.H. Kuhn, Nucl. Phys. B314(1989)1
C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
C
      Implicit real*8(a-h,o-z)
      data pi/3.14159265358979323846d0/
      REAL*8 mt,mw,wwidth,twidth
      REAL*8 front_factor,alpha,alpha_4
      REAL*8 mass_factor,pi_factor
C
C---------------------------------------------------------------------
C
      front_factor= 1.88392789543834161d-06*mt*mt*mt
      alpha = mw/mt
      alpha_4 = alpha*alpha*alpha*alpha
      mass_factor = (1.0 - 3.0*alpha_4 + 
     . 2.0*alpha_4*alpha*alpha)/(mw*wwidth)
      pi_factor = atan( ((mt-5.0)*(mt-5.0) - mw*mw)/(mw*wwidth) ) - 
     . atan(-mw/ wwidth)
      twidth = 9.11*front_factor*pi_factor*mass_factor
      END
C
C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
