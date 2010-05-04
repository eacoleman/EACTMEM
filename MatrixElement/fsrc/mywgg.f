      SUBROUTINE MYWGG(P1,MW,ANS)
C  
C Generated by MadGraph II Version 3.0. Updated 02/19/04                
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C AND HELICITIES
C FOR THE POINT IN PHASE SPACE P(0:3,NEXTERNAL)
C  
C FOR PROCESS : s~ g -> e+ ve c~ g
      IMPLICIT NONE
C  
C CONSTANTS
C  
      INTEGER                 NCOMB,     NCROSS,      NEXTERNAL        
      PARAMETER (             NCOMB=  64, NCROSS=  1, NEXTERNAL= 6)
C  
C ARGUMENTS 
C  
      REAL*8 P1(0:3,NEXTERNAL),ANS(NCROSS)
C  
C LOCAL VARIABLES 
C  
      INTEGER NHEL(NEXTERNAL,NCOMB),NTRY
      REAL*8 T, P(0:3,NEXTERNAL), MW
      REAL*8 MATRIX_WGG
      INTEGER IHEL,IDEN(NCROSS),IC(NEXTERNAL,NCROSS)
      INTEGER IPROC, I
      LOGICAL GOODHEL(NCOMB,NCROSS)
C  
C GLOBAL VARIABLES
C  

      DATA (NHEL(IHEL,   1),IHEL=1,6) /-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,   2),IHEL=1,6) /-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,   3),IHEL=1,6) /-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,   4),IHEL=1,6) /-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,   5),IHEL=1,6) /-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,   6),IHEL=1,6) /-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,   7),IHEL=1,6) /-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,   8),IHEL=1,6) /-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,   9),IHEL=1,6) /-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  10),IHEL=1,6) /-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  11),IHEL=1,6) /-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  12),IHEL=1,6) /-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  13),IHEL=1,6) /-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  14),IHEL=1,6) /-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  15),IHEL=1,6) /-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  16),IHEL=1,6) /-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  17),IHEL=1,6) /-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  18),IHEL=1,6) /-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  19),IHEL=1,6) /-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  20),IHEL=1,6) /-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  21),IHEL=1,6) /-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  22),IHEL=1,6) /-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  23),IHEL=1,6) /-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  24),IHEL=1,6) /-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  25),IHEL=1,6) /-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  26),IHEL=1,6) /-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  27),IHEL=1,6) /-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  28),IHEL=1,6) /-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  29),IHEL=1,6) /-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  30),IHEL=1,6) /-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  31),IHEL=1,6) /-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  32),IHEL=1,6) /-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  33),IHEL=1,6) / 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  34),IHEL=1,6) / 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  35),IHEL=1,6) / 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  36),IHEL=1,6) / 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  37),IHEL=1,6) / 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  38),IHEL=1,6) / 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  39),IHEL=1,6) / 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  40),IHEL=1,6) / 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  41),IHEL=1,6) / 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  42),IHEL=1,6) / 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  43),IHEL=1,6) / 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  44),IHEL=1,6) / 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  45),IHEL=1,6) / 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  46),IHEL=1,6) / 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  47),IHEL=1,6) / 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  48),IHEL=1,6) / 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  49),IHEL=1,6) / 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  50),IHEL=1,6) / 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  51),IHEL=1,6) / 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  52),IHEL=1,6) / 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  53),IHEL=1,6) / 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  54),IHEL=1,6) / 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  55),IHEL=1,6) / 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  56),IHEL=1,6) / 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  57),IHEL=1,6) / 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  58),IHEL=1,6) / 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  59),IHEL=1,6) / 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  60),IHEL=1,6) / 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  61),IHEL=1,6) / 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  62),IHEL=1,6) / 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  63),IHEL=1,6) / 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  64),IHEL=1,6) / 1, 1, 1, 1, 1, 1/
      DATA (  IC(IHEL,  1),IHEL=1,6) / 1, 2, 3, 4, 5, 6/
      DATA (IDEN(IHEL),IHEL=  1,  1) /  96/
C ----------
C BEGIN CODE
C ----------

      ANS(1) = 0D0
      DO IHEL=1,NCOMB
        T=MATRIX_WGG(P1,MW,NHEL(1,IHEL))		
        IF (T .GT. 0) THEN 
        ANS(1)=ANS(1)+T
C	  PRINT *,"IHEL, T ", IHEL,T
	ENDIF
      ENDDO
      
      ANS(1)=ANS(1)/DBLE(IDEN(1))
C      PRINT *," ME2 ",ANS(1)
C      CALL EXIT(1)
      END


       
       
      REAL*8 FUNCTION MATRIX_WGG(P,MW,NHEL)
C  
C Generated by MadGraph II Version 3.0. Updated 02/19/04                
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C FOR THE POINT WITH EXTERNAL LINES W(0:6,NEXTERNAL)
C  
C  
      IMPLICIT NONE
C  
C CONSTANTS
C  
      REAL*8     ZERO
      PARAMETER (ZERO=0D0)
      double precision  Pi, Fourpi
      parameter( Pi = 3.14159265358979323846d0 )
      parameter( Fourpi = 4.0d0  * Pi )
      INTEGER     NEXTERNAL,NWAVEFUNCS        
      PARAMETER ( NEXTERNAL= 6,NWAVEFUNCS=17)
C  
C ARGUMENTS 
C  
      REAL*8 P(0:3,NEXTERNAL),MW
      INTEGER NHEL(NEXTERNAL)
C  
C LOCAL VARIABLES 
C  
      COMPLEX*16 AMP(8),JAMP(2)
      COMPLEX*16 W(6,NWAVEFUNCS)
      double precision  sin2w,sw,alpha,ee,gw,g
      double precision  tmass,wmass,bmass,wwidth,twidth
      double complex gwf(2)   
      double complex gg(2)
C  
C COLOR DATA
C  
      INTEGER I,J,NCOLOR
      PARAMETER (NCOLOR=   2) 
      COMPLEX*16 ZTEMP
      REAL*8 DENOM(NCOLOR), CF(NCOLOR,NCOLOR)
      DATA Denom(1  )/            3/                                       
      DATA (CF(i,1  ),i=1  ,2  ) /    16,   -2/                            
C               T[1,5,6,2]                                                 
      DATA Denom(2  )/            3/                                       
      DATA (CF(i,2  ),i=1  ,2  ) /    -2,   16/                            
C               T[1,5,2,6]                                                 

      INTEGER II

C  
C assigning stuff
C  
      sin2w=0.23120 
      alpha = 1/128.9000    
      ee  = sqrt( alpha * Fourpi ) 

C     all this is set in setpara.f usually	                                            
      wmass   = MW !W  pole mass mw=80.419d0
      bmass   = 4.7d0    !bottom pole mass
      wwidth  = 2.12d0   ! PDG best value. Overidden if lwdecay=.true. 
      twidth  = 1.6d0 
      gwf(1) = dcmplx( -ee/sqrt(2.0d0*sin2w), Zero )
      gwf(2) = dcmplx(  Zero              , Zero )
      GG(1) = -1.2633009
      GG(2) = -1.2633009     
      G = 1.2633009     
C      PRINT *, GWF, WMASS, WWIDTH
C               T[5,1]T[2,6]                                               
C ----------
C BEGIN CODE
C ----------
      CALL IXXXXX(P(0,1   ),ZERO ,NHEL(1   ),+1,W(1,1   ))        
      CALL OXXXXX(P(0,2   ),ZERO ,NHEL(2   ),-1,W(1,2   ))        
      CALL IXXXXX(P(0,3   ),ZERO ,NHEL(3   ),-1,W(1,3   ))        
      CALL OXXXXX(P(0,4   ),ZERO ,NHEL(4   ),+1,W(1,4   ))        
      CALL VXXXXX(P(0,5   ),ZERO ,NHEL(5   ),+1,W(1,5   ))        
      CALL VXXXXX(P(0,6   ),ZERO ,NHEL(6   ),+1,W(1,6   ))        
      CALL JIOXXX(W(1,3   ),W(1,4   ),GWF ,WMASS   ,WWIDTH  ,W(1,7   ))    
      CALL FVOXXX(W(1,2   ),W(1,7   ),GWF ,ZERO    ,ZERO    ,W(1,8   ))    
      CALL FVOXXX(W(1,8   ),W(1,5   ),GG ,ZERO    ,ZERO    ,W(1,9   ))     
      CALL IOVXXX(W(1,1   ),W(1,9   ),W(1,6   ),GG ,AMP(1   ))             

      CALL FVIXXX(W(1,1   ),W(1,5   ),GG ,ZERO    ,ZERO    ,W(1,10  ))     
      CALL FVIXXX(W(1,10  ),W(1,7   ),GWF ,ZERO    ,ZERO    ,W(1,11  ))    
      CALL IOVXXX(W(1,11  ),W(1,2   ),W(1,6   ),GG ,AMP(2   ))             

      CALL JIOXXX(W(1,1   ),W(1,8   ),GG ,ZERO    ,ZERO    ,W(1,12  ))     
      CALL VVVXXX(W(1,6   ),W(1,5   ),W(1,12  ),G ,AMP(3   ))              

      CALL IOVXXX(W(1,10  ),W(1,8   ),W(1,6   ),GG ,AMP(4   ))             

      CALL FVOXXX(W(1,2   ),W(1,5   ),GG ,ZERO    ,ZERO    ,W(1,13  ))     
      CALL FVOXXX(W(1,13  ),W(1,7   ),GWF ,ZERO    ,ZERO    ,W(1,14  ))    
      CALL IOVXXX(W(1,1   ),W(1,14  ),W(1,6   ),GG ,AMP(5   ))             

      CALL FVIXXX(W(1,1   ),W(1,7   ),GWF ,ZERO    ,ZERO    ,W(1,15  ))    
      CALL FVIXXX(W(1,15  ),W(1,5   ),GG ,ZERO    ,ZERO    ,W(1,16  ))     
      CALL IOVXXX(W(1,16  ),W(1,2   ),W(1,6   ),GG ,AMP(6   ))             

      CALL JIOXXX(W(1,15  ),W(1,2   ),GG ,ZERO    ,ZERO    ,W(1,17  ))     
      CALL VVVXXX(W(1,6   ),W(1,5   ),W(1,17  ),G ,AMP(7   ))              

      CALL IOVXXX(W(1,15  ),W(1,13  ),W(1,6   ),GG ,AMP(8   ))             

      JAMP(   1) = +AMP(   1)-AMP(   3)+AMP(   5)-AMP(   7)+AMP(   8)
      JAMP(   2) = +AMP(   2)+AMP(   3)+AMP(   4)+AMP(   6)+AMP(   7)
      MATRIX_WGG = 0.D0 
      DO I = 1, NCOLOR
          ZTEMP = (0.D0,0.D0)
          DO J = 1, NCOLOR
              ZTEMP = ZTEMP + CF(J,I)*JAMP(J)
          ENDDO
          MATRIX_WGG =MATRIX_WGG+ZTEMP*DCONJG(JAMP(I))/DENOM(I)   
      ENDDO
C      Do I = 1, NGRAPHS
C          amp2(i)=amp2(i)+amp(i)*dconjg(amp(i))
C      Enddo
C      Do I = 1, NCOLOR
C          Jamp2(i)=Jamp2(i)+Jamp(i)*dconjg(Jamp(i))
C      Enddo

      END
