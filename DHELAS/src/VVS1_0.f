C     This File is Automatically generated by ALOHA 
C     The process calculated in this file is: 
C     Metric(1,2)
C     
      SUBROUTINE VVS1_0(V1,V2,S3,COUP,VERTEX)
      IMPLICIT NONE
      DOUBLE COMPLEX V1(*)
      DOUBLE COMPLEX V2(*)
      DOUBLE COMPLEX S3(*)
      DOUBLE COMPLEX COUP
      DOUBLE COMPLEX VERTEX


      VERTEX = COUP*(S3(1)*( (0, -1)*(V2(1)*V1(1))+(0, 1)*(V2(2)
     $ *V1(2))+(0, 1)*(V2(3)*V1(3))+(0, 1)*(V2(4)*V1(4))))
      END


