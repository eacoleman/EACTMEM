      SUBROUTINE MYSCHAN3JET(P1,MTOP,MW,ANS)
C  
C Generated by MadGraph II                                              
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C AND HELICITIES
C FOR THE POINT IN PHASE SPACE P(0:3,7)
C  
C FOR PROCESS : u d~ -> e+ ve b b~ g  
C  
C Crossing   1 is u d~ -> e+ ve b b~ g  
      IMPLICIT NONE
C  
C CONSTANTS
C  
C      Include "genps.inc"
      INTEGER                 NCOMB,     NCROSS         
      PARAMETER (             NCOMB= 128, NCROSS=  1)
C      INTEGER    THEL
C      PARAMETER (THEL=NCOMB*NCROSS)
C  
C ARGUMENTS 
C  
      REAL*8 P1(0:3,7),ANS(NCROSS)
C  
C LOCAL VARIABLES 
C  
      INTEGER NHEL(7,NCOMB),NTRY
      REAL*8 T, P(0:3,7), MTOP, MW
      REAL*8 MMYSCHAN3JET
      INTEGER IHEL,I
C      INTEGER IPROC,JC(7), I
      LOGICAL GOODHEL(NCOMB,NCROSS)
      INTEGER NGRAPHS
C      REAL*8 hwgt, xtot, xtry, xrej, xr, yfrac(0:ncomb)
C      INTEGER idum, ngood, igood(ncomb), jhel, j
C      LOGICAL warned
C      REAL     xran1
C      EXTERNAL xran1
C  
C GLOBAL VARIABLES
C  
C      Double Precision amp2(maxamps), jamp2(0:maxamps)
C      common/to_amps/  amp2,       jamp2
C
C      character*79         hel_buff
C      common/to_helicity/  hel_buff
C
C      integer          isum_hel
C      logical                    multi_channel
C      common/to_matrix/isum_hel, multi_channel
C      INTEGER MAPCONFIG(0:LMAXCONFIGS), ICONFIG
C      common/to_mconfigs/mapconfig, iconfig
C      DATA NTRY,IDUM /0,-1/
C      DATA xtry, xrej, ngood /0,0,0/
C      DATA warned, isum_hel/.false.,0/
C      DATA multi_channel/.true./
C      SAVE yfrac, igood, jhel
C      DATA NGRAPHS /    5/          
C      DATA jamp2(0) /   2/          
C      DATA GOODHEL/THEL*.FALSE./
      DATA (NHEL(IHEL,   1),IHEL=1,7) /-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,   2),IHEL=1,7) /-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,   3),IHEL=1,7) /-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,   4),IHEL=1,7) /-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,   5),IHEL=1,7) /-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,   6),IHEL=1,7) /-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,   7),IHEL=1,7) /-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,   8),IHEL=1,7) /-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,   9),IHEL=1,7) /-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  10),IHEL=1,7) /-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  11),IHEL=1,7) /-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  12),IHEL=1,7) /-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  13),IHEL=1,7) /-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  14),IHEL=1,7) /-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  15),IHEL=1,7) /-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  16),IHEL=1,7) /-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  17),IHEL=1,7) /-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  18),IHEL=1,7) /-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  19),IHEL=1,7) /-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  20),IHEL=1,7) /-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  21),IHEL=1,7) /-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  22),IHEL=1,7) /-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  23),IHEL=1,7) /-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  24),IHEL=1,7) /-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  25),IHEL=1,7) /-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  26),IHEL=1,7) /-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  27),IHEL=1,7) /-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  28),IHEL=1,7) /-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  29),IHEL=1,7) /-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  30),IHEL=1,7) /-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  31),IHEL=1,7) /-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  32),IHEL=1,7) /-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  33),IHEL=1,7) /-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  34),IHEL=1,7) /-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  35),IHEL=1,7) /-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  36),IHEL=1,7) /-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  37),IHEL=1,7) /-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  38),IHEL=1,7) /-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  39),IHEL=1,7) /-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  40),IHEL=1,7) /-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  41),IHEL=1,7) /-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  42),IHEL=1,7) /-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  43),IHEL=1,7) /-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  44),IHEL=1,7) /-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  45),IHEL=1,7) /-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  46),IHEL=1,7) /-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  47),IHEL=1,7) /-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  48),IHEL=1,7) /-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  49),IHEL=1,7) /-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  50),IHEL=1,7) /-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  51),IHEL=1,7) /-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  52),IHEL=1,7) /-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  53),IHEL=1,7) /-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  54),IHEL=1,7) /-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  55),IHEL=1,7) /-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  56),IHEL=1,7) /-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  57),IHEL=1,7) /-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  58),IHEL=1,7) /-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  59),IHEL=1,7) /-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  60),IHEL=1,7) /-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  61),IHEL=1,7) /-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  62),IHEL=1,7) /-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  63),IHEL=1,7) /-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  64),IHEL=1,7) /-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  65),IHEL=1,7) / 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  66),IHEL=1,7) / 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  67),IHEL=1,7) / 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  68),IHEL=1,7) / 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  69),IHEL=1,7) / 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  70),IHEL=1,7) / 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  71),IHEL=1,7) / 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  72),IHEL=1,7) / 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  73),IHEL=1,7) / 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  74),IHEL=1,7) / 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  75),IHEL=1,7) / 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  76),IHEL=1,7) / 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  77),IHEL=1,7) / 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  78),IHEL=1,7) / 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  79),IHEL=1,7) / 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  80),IHEL=1,7) / 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  81),IHEL=1,7) / 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  82),IHEL=1,7) / 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  83),IHEL=1,7) / 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  84),IHEL=1,7) / 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  85),IHEL=1,7) / 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  86),IHEL=1,7) / 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  87),IHEL=1,7) / 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  88),IHEL=1,7) / 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  89),IHEL=1,7) / 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  90),IHEL=1,7) / 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  91),IHEL=1,7) / 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  92),IHEL=1,7) / 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  93),IHEL=1,7) / 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  94),IHEL=1,7) / 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  95),IHEL=1,7) / 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  96),IHEL=1,7) / 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  97),IHEL=1,7) / 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  98),IHEL=1,7) / 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  99),IHEL=1,7) / 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 100),IHEL=1,7) / 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 101),IHEL=1,7) / 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 102),IHEL=1,7) / 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 103),IHEL=1,7) / 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 104),IHEL=1,7) / 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 105),IHEL=1,7) / 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 106),IHEL=1,7) / 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 107),IHEL=1,7) / 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 108),IHEL=1,7) / 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 109),IHEL=1,7) / 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 110),IHEL=1,7) / 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 111),IHEL=1,7) / 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 112),IHEL=1,7) / 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 113),IHEL=1,7) / 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 114),IHEL=1,7) / 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 115),IHEL=1,7) / 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 116),IHEL=1,7) / 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 117),IHEL=1,7) / 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 118),IHEL=1,7) / 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 119),IHEL=1,7) / 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 120),IHEL=1,7) / 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 121),IHEL=1,7) / 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 122),IHEL=1,7) / 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 123),IHEL=1,7) / 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 124),IHEL=1,7) / 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 125),IHEL=1,7) / 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 126),IHEL=1,7) / 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 127),IHEL=1,7) / 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 128),IHEL=1,7) / 1, 1, 1, 1, 1, 1, 1/
C      DATA (  IC(IHEL,  1),IHEL=1,7) / 1, 2, 3, 4, 5, 6, 7/
C      DATA (IDEN(IHEL),IHEL=  1,  1) /  36/
C ----------
C BEGIN CODE
C ----------
C      NTRY=NTRY+1
C      DO IPROC=1,NCROSS
C        CALL SWITCHMOM(P1,P,IC(1,IPROC),JC,7)
C         DO IHEL=1,7
C         JC(IHEL) = +1
C      ENDDO
      
C      IF (multi_channel) THEN
C         DO IHEL=1,NGRAPHS
C            amp2(ihel)=0d0
C            jamp2(ihel)=0d0
C         ENDDO
C         DO IHEL=1,int(jamp2(0))
C              jamp2(ihel)=0d0
C           ENDDO
C        ENDIF
        ANS(1) = 0D0
C      write(hel_buff,'(16i5)') (0,i=1,7)
C      IF (ISUM_HEL .EQ. 0 .OR. NTRY .LT. 10) THEN
         DO IHEL=1,NCOMB
C         DO IHEL=49,56
C            IF (GOODHEL(IHEL,IPROC) .OR. NTRY .LT. 2) THEN
C      PRINT *,"Combining"
               T=MMYSCHAN3JET(P1 ,MTOP, MW,NHEL(1,IHEL))            
               ANS(1)=ANS(1)+T
               IF (T .GT. 0D0) THEN
C               PRINT *, " IHEL T ", IHEL, T
            ENDIF
C               IF (T .GT. 0D0 .AND. .NOT. GOODHEL(IHEL,IPROC)) THEN
C                  GOODHEL(IHEL,IPROC)=.TRUE.
C                  NGOOD = NGOOD +1
C                  IGOOD(NGOOD) = IHEL
C      WRITE(*,*) ngood,IHEL,T
C               ENDIF
C            ENDIF
          ENDDO
C          JHEL = 1
C          ISUM_HEL=MIN(ISUM_HEL,NGOOD)
C       ELSE                     !RANDOM HELICITY
C          DO J=1,ISUM_HEL
C             JHEL=JHEL+1
C             IF (JHEL .GT. NGOOD) JHEL=1
C             HWGT = REAL(NGOOD)/REAL(ISUM_HEL)
C             IHEL = IGOOD(JHEL)
C             T=MATRIX(P ,NHEL(1,IHEL),JC(1))            
C             ANS(IPROC)=ANS(IPROC)+T*HWGT
C          ENDDO
C          IF (ISUM_HEL .EQ. 1) THEN
C             WRITE(HEL_BUFF,'(16i5)')(NHEL(i,IHEL),i=1,nexternal)
C          ENDIF
C       ENDIF
C       IF (MULTI_CHANNEL) THEN
C          XTOT=0D0
C          DO IHEL=1,MAPCONFIG(0)
C              XTOT=XTOT+AMP2(MAPCONFIG(IHEL))
C           ENDDO
C          ANS(IPROC)=ANS(IPROC)*AMP2(MAPCONFIG(ICONFIG))/XTOT
C       ENDIF
C       ANS(IPROC)=ANS(IPROC)
C      ENDDO
C          PRINT *, " ME for schan ", ANS(1)
C          CALL EXIT(0)
      END
      
       
      REAL*8 FUNCTION MMYSCHAN3JET(P,MTOP,MW,NHEL)
C  
C Generated by MadGraph II                                              
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C FOR THE POINT WITH EXTERNAL LINES W(0:6,7)
C  
C FOR PROCESS : u d~ -> e+ ve b b~ g  
C  
      IMPLICIT NONE
C  
C CONSTANTS
C  
      INTEGER    NGRAPHS,    NEIGEN 
      PARAMETER (NGRAPHS=   5,NEIGEN=  2) 
C      include "genps.inc"
      INTEGER    NWAVEFUNCS     , NCOLOR
      PARAMETER (NWAVEFUNCS=  18, NCOLOR=   2) 
C      REAL*8     ZERO
C      PARAMETER (ZERO=0D0)
      REAL*8     ZERO
      PARAMETER (ZERO=0D0)
      double precision  Pi, Fourpi, Rt2, gfermi
      parameter( Pi = 3.14159265358979323846d0 )
      parameter( Fourpi = 4.0d0  * Pi )
      parameter( Rt2   = 1.414213562d0 )
      parameter( gfermi = 1.16639d-5 )

C  
C ARGUMENTS 
C  
      REAL*8 P(0:3,7),MTOP,MW
      INTEGER NHEL(7)
C  
C LOCAL VARIABLES 
C  
      INTEGER I,J
      COMPLEX*16 ZTEMP
      REAL*8 DENOM(NCOLOR), CF(NCOLOR,NCOLOR)
      COMPLEX*16 AMP(NGRAPHS), JAMP(NCOLOR)
      COMPLEX*16 W(18,NWAVEFUNCS)
      double precision  sin2w,sw,alpha,ee,gw,g,alfas
      double precision  tmass,wmass,bmass,wwidth,twidth
      double complex gwf(2), gg(2)

      tmass   = MTOP  !top pole mass
      wmass   = MW !W  pole mass mw=80.419d0
      bmass   = 4.7d0    !bottom pole mass
      wwidth  = 2.04759d0   
      sin2w=0.23120 
      sw  = sqrt( sin2w )
      alpha  = Rt2*gfermi*wmass**2*sin2w/pi      
      ee  = sqrt( alpha * Fourpi ) 
      gw   = ee/sw
C      PRINT *," 2 alpha, wmass ",alpha,wmass
      gwf(1) = dcmplx( -ee/sqrt(2.0d0*sin2w), Zero )
      gwf(2) = dcmplx(  Zero              , Zero )
      call mytopwidth(tmass,wmass,wwidth,twidth)
C      PRINT *, "Top width: ", twidth
      ALFAS = 0.119d0
      G = DSQRT(4d0*PI*ALFAS) ! use setting of the param_card.dat @ NLO
      GG(1) = -G
      GG(2) = -G     

C  
C GLOBAL VARIABLES
C  
C      Double Precision amp2(maxamps), jamp2(0:maxamps)
C      common/to_amps/  amp2,       jamp2
C      include "coupl.inc"
C  
C COLOR DATA
C  
C      DATA Denom(1  )/            1/                                       
C      DATA (CF(i,1  ),i=1  ,2  ) /    12,    0/                            
C               T[2,1,7]T[5,6]                                             
C      DATA Denom(2  )/            1/                                       
C      DATA (CF(i,2  ),i=1  ,2  ) /     0,   12/                            
C               T[2,1]T[5,6,7]                                             
C ----------
C BEGIN CODE
C ----------
C      PRINT *, "P: ", P
      CALL IXXXXX(P(0,1   ),ZERO ,NHEL(1   ),+1,W(1,1   ))        
      CALL OXXXXX(P(0,2   ),ZERO ,NHEL(2   ),-1,W(1,2   ))        
      CALL IXXXXX(P(0,3   ),ZERO ,NHEL(3   ),-1,W(1,3   ))        
      CALL OXXXXX(P(0,4   ),ZERO ,NHEL(4   ),+1,W(1,4   ))        
      CALL OXXXXX(P(0,5   ),BMASS ,NHEL(5   ),+1,W(1,5   ))       
      CALL IXXXXX(P(0,6   ),BMASS ,NHEL(6   ),-1,W(1,6   ))       
      CALL VXXXXX(P(0,7   ),ZERO ,NHEL(7   ),+1,W(1,7   ))        
C      PRINT *,"W: ", W(1,1), W(1,2), W(1,3), W(1,4), W(1,5), W(1,6),
C     .W(1,7)
      CALL JIOXXX(W(1,3   ),W(1,4   ),GWF ,WMASS   ,WWIDTH  ,W(1,8   ))    
      CALL FVOXXX(W(1,5   ),W(1,8   ),GWF ,TMASS   ,TWIDTH  ,W(1,9   ))    
      CALL FVIXXX(W(1,1   ),W(1,7   ),GG ,ZERO    ,ZERO    ,W(1,10  ))     
      CALL JIOXXX(W(1,10  ),W(1,2   ),GWF ,WMASS   ,WWIDTH  ,W(1,11  ))    
      CALL IOVXXX(W(1,6   ),W(1,9   ),W(1,11  ),GWF ,AMP(1   ))            

      CALL FVOXXX(W(1,2   ),W(1,7   ),GG ,ZERO    ,ZERO    ,W(1,12  ))     
      CALL JIOXXX(W(1,1   ),W(1,12  ),GWF ,WMASS   ,WWIDTH  ,W(1,13  ))    
      CALL IOVXXX(W(1,6   ),W(1,9   ),W(1,13  ),GWF ,AMP(2   ))            

      CALL JIOXXX(W(1,1   ),W(1,2   ),GWF ,WMASS   ,WWIDTH  ,W(1,14  ))    
      CALL FVOXXX(W(1,5   ),W(1,7   ),GG ,BMASS   ,ZERO    ,W(1,15  ))     
      CALL FVOXXX(W(1,15  ),W(1,8   ),GWF ,TMASS   ,TWIDTH  ,W(1,16  ))    
      CALL IOVXXX(W(1,6   ),W(1,16  ),W(1,14  ),GWF ,AMP(3   ))            

      CALL FVOXXX(W(1,9   ),W(1,14  ),GWF ,BMASS   ,ZERO    ,W(1,17  ))    
      CALL IOVXXX(W(1,6   ),W(1,17  ),W(1,7   ),GG ,AMP(4   ))             

      CALL FVOXXX(W(1,9   ),W(1,7   ),GG ,TMASS   ,TWIDTH  ,W(1,18  ))     
      CALL IOVXXX(W(1,6   ),W(1,18  ),W(1,14  ),GWF ,AMP(5   ))            
C      PRINT *, "Your"
C      PRINT *, "AMP: ", AMP
C      CALL EXIT(1)
      JAMP(   1) = -AMP(   1)-AMP(   2)
      JAMP(   2) = -AMP(   3)-AMP(   4)-AMP(   5)
C      PRINT *, "JAMP: ", JAMP
      MMYSCHAN3JET = 0.D0 
C      PRINT *, "mom"
C      DO I = 1, NCOLOR
C          ZTEMP = (0.D0,0.D0)
CC          PRINT *, "is"
C          DO J = 1, NCOLOR
C              ZTEMP = ZTEMP + CF(J,I)*JAMP(J)
C          ENDDO
CC          PRINT *, "a"
C          MMYSCHAN3JET =MMYSCHAN3JET+ZTEMP*DCONJG(JAMP(I))/DENOM(I) 
CC          PRINT *, "FINAL: ",MMYSCHAN3JET
C      ENDDO
          DO J = 1, NCOLOR
             MMYSCHAN3JET =MMYSCHAN3JET + JAMP(J) * DCONJG(JAMP(J)) * 12
          ENDDO
C      Do I = 1, NGRAPHS
C          amp2(i)=amp2(i)+amp(i)*dconjg(amp(i))
C      Enddo
C      Do I = 1, NCOLOR
C          Jamp2(i)=Jamp2(i)+Jamp(i)*dconjg(Jamp(i))
C      Enddo
C      CALL GAUGECHECK(JAMP,ZTEMP,EIGEN_VEC,EIGEN_VAL,NCOLOR,NEIGEN) 
      END
       
       
