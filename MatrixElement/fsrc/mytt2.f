      SUBROUTINE MYTT2(P1,MTOP,MW,ANS)
C  
C Generated by MadGraph II Version 3.0. Updated 02/19/04                
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C AND HELICITIES
C FOR THE POINT IN PHASE SPACE P(0:3,4)
C  
C FOR PROCESS : u u~ -> e+ ve b b~ w-  
C  
      IMPLICIT NONE
C  
C CONSTANTS
C  
      INTEGER                 NCOMB,     NCROSS         
      PARAMETER (NCOMB=  192, NCROSS=  1)
C  
C ARGUMENTS 
C  
      REAL*8 P1(0:3,7),ANS(NCROSS)
C  
C LOCAL VARIABLES 
C  
      INTEGER NHEL(7,NCOMB),NTRY
      REAL*8 T, P(0:3,7), MTOP, MW
      REAL*8 MMYTT2
      INTEGER IHEL,IDEN(NCROSS),IC(7,NCROSS)
      INTEGER IPROC, I
      LOGICAL GOODHEL(NCOMB,NCROSS)
C  
C GLOBAL VARIABLES
C  

      DATA (NHEL(IHEL,   1),IHEL=1,7) /-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,   2),IHEL=1,7) /-1,-1,-1,-1,-1,-1, 0/
      DATA (NHEL(IHEL,   3),IHEL=1,7) /-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,   4),IHEL=1,7) /-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,   5),IHEL=1,7) /-1,-1,-1,-1,-1, 1, 0/
      DATA (NHEL(IHEL,   6),IHEL=1,7) /-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,   7),IHEL=1,7) /-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,   8),IHEL=1,7) /-1,-1,-1,-1, 1,-1, 0/
      DATA (NHEL(IHEL,   9),IHEL=1,7) /-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  10),IHEL=1,7) /-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  11),IHEL=1,7) /-1,-1,-1,-1, 1, 1, 0/
      DATA (NHEL(IHEL,  12),IHEL=1,7) /-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  13),IHEL=1,7) /-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  14),IHEL=1,7) /-1,-1,-1, 1,-1,-1, 0/
      DATA (NHEL(IHEL,  15),IHEL=1,7) /-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  16),IHEL=1,7) /-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  17),IHEL=1,7) /-1,-1,-1, 1,-1, 1, 0/
      DATA (NHEL(IHEL,  18),IHEL=1,7) /-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  19),IHEL=1,7) /-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  20),IHEL=1,7) /-1,-1,-1, 1, 1,-1, 0/
      DATA (NHEL(IHEL,  21),IHEL=1,7) /-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  22),IHEL=1,7) /-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  23),IHEL=1,7) /-1,-1,-1, 1, 1, 1, 0/
      DATA (NHEL(IHEL,  24),IHEL=1,7) /-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  25),IHEL=1,7) /-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  26),IHEL=1,7) /-1,-1, 1,-1,-1,-1, 0/
      DATA (NHEL(IHEL,  27),IHEL=1,7) /-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  28),IHEL=1,7) /-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  29),IHEL=1,7) /-1,-1, 1,-1,-1, 1, 0/
      DATA (NHEL(IHEL,  30),IHEL=1,7) /-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  31),IHEL=1,7) /-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  32),IHEL=1,7) /-1,-1, 1,-1, 1,-1, 0/
      DATA (NHEL(IHEL,  33),IHEL=1,7) /-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  34),IHEL=1,7) /-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  35),IHEL=1,7) /-1,-1, 1,-1, 1, 1, 0/
      DATA (NHEL(IHEL,  36),IHEL=1,7) /-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  37),IHEL=1,7) /-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  38),IHEL=1,7) /-1,-1, 1, 1,-1,-1, 0/
      DATA (NHEL(IHEL,  39),IHEL=1,7) /-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  40),IHEL=1,7) /-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  41),IHEL=1,7) /-1,-1, 1, 1,-1, 1, 0/
      DATA (NHEL(IHEL,  42),IHEL=1,7) /-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  43),IHEL=1,7) /-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  44),IHEL=1,7) /-1,-1, 1, 1, 1,-1, 0/
      DATA (NHEL(IHEL,  45),IHEL=1,7) /-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  46),IHEL=1,7) /-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  47),IHEL=1,7) /-1,-1, 1, 1, 1, 1, 0/
      DATA (NHEL(IHEL,  48),IHEL=1,7) /-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  49),IHEL=1,7) /-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  50),IHEL=1,7) /-1, 1,-1,-1,-1,-1, 0/
      DATA (NHEL(IHEL,  51),IHEL=1,7) /-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  52),IHEL=1,7) /-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  53),IHEL=1,7) /-1, 1,-1,-1,-1, 1, 0/
      DATA (NHEL(IHEL,  54),IHEL=1,7) /-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  55),IHEL=1,7) /-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  56),IHEL=1,7) /-1, 1,-1,-1, 1,-1, 0/
      DATA (NHEL(IHEL,  57),IHEL=1,7) /-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  58),IHEL=1,7) /-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  59),IHEL=1,7) /-1, 1,-1,-1, 1, 1, 0/
      DATA (NHEL(IHEL,  60),IHEL=1,7) /-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  61),IHEL=1,7) /-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  62),IHEL=1,7) /-1, 1,-1, 1,-1,-1, 0/
      DATA (NHEL(IHEL,  63),IHEL=1,7) /-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  64),IHEL=1,7) /-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  65),IHEL=1,7) /-1, 1,-1, 1,-1, 1, 0/
      DATA (NHEL(IHEL,  66),IHEL=1,7) /-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  67),IHEL=1,7) /-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  68),IHEL=1,7) /-1, 1,-1, 1, 1,-1, 0/
      DATA (NHEL(IHEL,  69),IHEL=1,7) /-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  70),IHEL=1,7) /-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  71),IHEL=1,7) /-1, 1,-1, 1, 1, 1, 0/
      DATA (NHEL(IHEL,  72),IHEL=1,7) /-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  73),IHEL=1,7) /-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  74),IHEL=1,7) /-1, 1, 1,-1,-1,-1, 0/
      DATA (NHEL(IHEL,  75),IHEL=1,7) /-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  76),IHEL=1,7) /-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  77),IHEL=1,7) /-1, 1, 1,-1,-1, 1, 0/
      DATA (NHEL(IHEL,  78),IHEL=1,7) /-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  79),IHEL=1,7) /-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  80),IHEL=1,7) /-1, 1, 1,-1, 1,-1, 0/
      DATA (NHEL(IHEL,  81),IHEL=1,7) /-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  82),IHEL=1,7) /-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  83),IHEL=1,7) /-1, 1, 1,-1, 1, 1, 0/
      DATA (NHEL(IHEL,  84),IHEL=1,7) /-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  85),IHEL=1,7) /-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  86),IHEL=1,7) /-1, 1, 1, 1,-1,-1, 0/
      DATA (NHEL(IHEL,  87),IHEL=1,7) /-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  88),IHEL=1,7) /-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  89),IHEL=1,7) /-1, 1, 1, 1,-1, 1, 0/
      DATA (NHEL(IHEL,  90),IHEL=1,7) /-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  91),IHEL=1,7) /-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  92),IHEL=1,7) /-1, 1, 1, 1, 1,-1, 0/
      DATA (NHEL(IHEL,  93),IHEL=1,7) /-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  94),IHEL=1,7) /-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  95),IHEL=1,7) /-1, 1, 1, 1, 1, 1, 0/
      DATA (NHEL(IHEL,  96),IHEL=1,7) /-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  97),IHEL=1,7) / 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  98),IHEL=1,7) / 1,-1,-1,-1,-1,-1, 0/
      DATA (NHEL(IHEL,  99),IHEL=1,7) / 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 100),IHEL=1,7) / 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 101),IHEL=1,7) / 1,-1,-1,-1,-1, 1, 0/
      DATA (NHEL(IHEL, 102),IHEL=1,7) / 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 103),IHEL=1,7) / 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 104),IHEL=1,7) / 1,-1,-1,-1, 1,-1, 0/
      DATA (NHEL(IHEL, 105),IHEL=1,7) / 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 106),IHEL=1,7) / 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 107),IHEL=1,7) / 1,-1,-1,-1, 1, 1, 0/
      DATA (NHEL(IHEL, 108),IHEL=1,7) / 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 109),IHEL=1,7) / 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 110),IHEL=1,7) / 1,-1,-1, 1,-1,-1, 0/
      DATA (NHEL(IHEL, 111),IHEL=1,7) / 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 112),IHEL=1,7) / 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 113),IHEL=1,7) / 1,-1,-1, 1,-1, 1, 0/
      DATA (NHEL(IHEL, 114),IHEL=1,7) / 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 115),IHEL=1,7) / 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 116),IHEL=1,7) / 1,-1,-1, 1, 1,-1, 0/
      DATA (NHEL(IHEL, 117),IHEL=1,7) / 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 118),IHEL=1,7) / 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 119),IHEL=1,7) / 1,-1,-1, 1, 1, 1, 0/
      DATA (NHEL(IHEL, 120),IHEL=1,7) / 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 121),IHEL=1,7) / 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 122),IHEL=1,7) / 1,-1, 1,-1,-1,-1, 0/
      DATA (NHEL(IHEL, 123),IHEL=1,7) / 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 124),IHEL=1,7) / 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 125),IHEL=1,7) / 1,-1, 1,-1,-1, 1, 0/
      DATA (NHEL(IHEL, 126),IHEL=1,7) / 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 127),IHEL=1,7) / 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 128),IHEL=1,7) / 1,-1, 1,-1, 1,-1, 0/
      DATA (NHEL(IHEL, 129),IHEL=1,7) / 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 130),IHEL=1,7) / 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 131),IHEL=1,7) / 1,-1, 1,-1, 1, 1, 0/
      DATA (NHEL(IHEL, 132),IHEL=1,7) / 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 133),IHEL=1,7) / 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 134),IHEL=1,7) / 1,-1, 1, 1,-1,-1, 0/
      DATA (NHEL(IHEL, 135),IHEL=1,7) / 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 136),IHEL=1,7) / 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 137),IHEL=1,7) / 1,-1, 1, 1,-1, 1, 0/
      DATA (NHEL(IHEL, 138),IHEL=1,7) / 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 139),IHEL=1,7) / 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 140),IHEL=1,7) / 1,-1, 1, 1, 1,-1, 0/
      DATA (NHEL(IHEL, 141),IHEL=1,7) / 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 142),IHEL=1,7) / 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 143),IHEL=1,7) / 1,-1, 1, 1, 1, 1, 0/
      DATA (NHEL(IHEL, 144),IHEL=1,7) / 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 145),IHEL=1,7) / 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 146),IHEL=1,7) / 1, 1,-1,-1,-1,-1, 0/
      DATA (NHEL(IHEL, 147),IHEL=1,7) / 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 148),IHEL=1,7) / 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 149),IHEL=1,7) / 1, 1,-1,-1,-1, 1, 0/
      DATA (NHEL(IHEL, 150),IHEL=1,7) / 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 151),IHEL=1,7) / 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 152),IHEL=1,7) / 1, 1,-1,-1, 1,-1, 0/
      DATA (NHEL(IHEL, 153),IHEL=1,7) / 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 154),IHEL=1,7) / 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 155),IHEL=1,7) / 1, 1,-1,-1, 1, 1, 0/
      DATA (NHEL(IHEL, 156),IHEL=1,7) / 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 157),IHEL=1,7) / 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 158),IHEL=1,7) / 1, 1,-1, 1,-1,-1, 0/
      DATA (NHEL(IHEL, 159),IHEL=1,7) / 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 160),IHEL=1,7) / 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 161),IHEL=1,7) / 1, 1,-1, 1,-1, 1, 0/
      DATA (NHEL(IHEL, 162),IHEL=1,7) / 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 163),IHEL=1,7) / 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 164),IHEL=1,7) / 1, 1,-1, 1, 1,-1, 0/
      DATA (NHEL(IHEL, 165),IHEL=1,7) / 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 166),IHEL=1,7) / 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 167),IHEL=1,7) / 1, 1,-1, 1, 1, 1, 0/
      DATA (NHEL(IHEL, 168),IHEL=1,7) / 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 169),IHEL=1,7) / 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 170),IHEL=1,7) / 1, 1, 1,-1,-1,-1, 0/
      DATA (NHEL(IHEL, 171),IHEL=1,7) / 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 172),IHEL=1,7) / 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 173),IHEL=1,7) / 1, 1, 1,-1,-1, 1, 0/
      DATA (NHEL(IHEL, 174),IHEL=1,7) / 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 175),IHEL=1,7) / 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 176),IHEL=1,7) / 1, 1, 1,-1, 1,-1, 0/
      DATA (NHEL(IHEL, 177),IHEL=1,7) / 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 178),IHEL=1,7) / 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 179),IHEL=1,7) / 1, 1, 1,-1, 1, 1, 0/
      DATA (NHEL(IHEL, 180),IHEL=1,7) / 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 181),IHEL=1,7) / 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 182),IHEL=1,7) / 1, 1, 1, 1,-1,-1, 0/
      DATA (NHEL(IHEL, 183),IHEL=1,7) / 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 184),IHEL=1,7) / 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 185),IHEL=1,7) / 1, 1, 1, 1,-1, 1, 0/
      DATA (NHEL(IHEL, 186),IHEL=1,7) / 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 187),IHEL=1,7) / 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 188),IHEL=1,7) / 1, 1, 1, 1, 1,-1, 0/
      DATA (NHEL(IHEL, 189),IHEL=1,7) / 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 190),IHEL=1,7) / 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 191),IHEL=1,7) / 1, 1, 1, 1, 1, 1, 0/
      DATA (NHEL(IHEL, 192),IHEL=1,7) / 1, 1, 1, 1, 1, 1, 1/
      DATA (  IC(IHEL,  1),IHEL=1,7) / 1, 2, 3, 4, 5, 6, 7/
      DATA (IDEN(IHEL),IHEL=  1,  1) /  36/
      
C      PRINT *," 1 P1(E,px,py,pz) ",P1(0,1),P1(1,1),P1(2,1),P1(3,1)
C      PRINT *," 1 P2(E,px,py,pz) ",P1(0,2),P1(1,2),P1(2,2),P1(3,2)
C      PRINT *," 1 P3(E,px,py,pz) ",P1(0,3),P1(1,3),P1(2,3),P1(3,3)
C      PRINT *," 1 P4(E,px,py,pz) ",P1(0,4),P1(1,4),P1(2,4),P1(3,4)
C      PRINT *," 1 P5(E,px,py,pz) ",P1(0,5),P1(1,5),P1(2,5),P1(3,5)
C      PRINT *," 1 P6(E,px,py,pz) ",P1(0,6),P1(1,6),P1(2,6),P1(3,6)
C      PRINT *," 1 P7(E,px,py,pz) ",P1(0,7),P1(1,7),P1(2,7),P1(3,7)
      
      
      ANS(1) = 0D0
      DO IHEL=1,NCOMB
        T=MMYTT2(P1 ,MTOP,MW,NHEL(1,IHEL))		
        IF (T .GT. 0) THEN 
C	  PRINT *,"IHEL, T ", IHEL,T
	ENDIF
        ANS(1)=ANS(1)+T
      ENDDO
      
      ANS(1)=ANS(1)/DBLE(IDEN(1))
      PRINT *," ME2 for MTOP",ANS(1), MTOP
      CALL EXIT(1)
      END

      REAL*8 FUNCTION MMYTT2(P,MTOP,MW,NHEL)
C  
C Generated by MadGraph II Version 3.0. Updated 02/19/04                
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C FOR THE POINT WITH EXTERNAL LINES W(0:6,4)
C  
C FOR PROCESS : u d~ -> e+ ve b b~  
C  
      IMPLICIT NONE
C  
C CONSTANTS
C  
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
      COMPLEX*16 AMP(1), JAMP(1)
      COMPLEX*16 W(6,11)
      double precision  sin2w,sw,alpha,ee,gw
      double precision  tmass,wmass,bmass,wwidth,twidth
      double complex gwf(2)   
      double complex gg(2)

C     all this is set in setpara.f usually	                                            
C     tmass   = 174.3d0  !top pole mass
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
      GG(1) = -1.2633009
      GG(2) = -1.2633009     
C      twidth  = 1.6d0 
C      call topwid(tmass,wmass,bmass,wwidth,gw,twidth)
      call mytopwidth(tmass,wmass,wwidth,twidth)
C      call topwidth(tmass,wmass,alpha,twidth)
C      PRINT *," tmass, twidth ",tmass,twidth

C ----------
C BEGIN CODE
C ----------
C       PRINT *," P1(E,px,py,pz) ",P(0,1),P(1,1),P(2,1),P(3,1)
C       PRINT *," P2(E,px,py,pz) ",P(0,2),P(1,2),P(2,2),P(3,2)
C       PRINT *," P3(E,px,py,pz) ",P(0,3),P(1,3),P(2,3),P(3,3)
C       PRINT *," P4(E,px,py,pz) ",P(0,4),P(1,4),P(2,4),P(3,4)
C       PRINT *," P5(E,px,py,pz) ",P(0,5),P(1,5),P(2,5),P(3,5)
C       PRINT *," P6(E,px,py,pz) ",P(0,6),P(1,6),P(2,6),P(3,6)
 
      CALL IXXXXX(P(0,1   ),ZERO ,NHEL(1   ),+1 ,W(1,1   ))        
      CALL OXXXXX(P(0,2   ),ZERO ,NHEL(2   ),-1 ,W(1,2   ))        
      CALL OXXXXX(P(0,3   ),ZERO ,NHEL(3   ),+1 ,W(1,3   ))        
      CALL IXXXXX(P(0,4   ),ZERO ,NHEL(4   ),-1 ,W(1,4   ))        
      CALL IXXXXX(P(0,5   ),BMASS ,NHEL(5   ),-1,W(1,5   ))       
      CALL OXXXXX(P(0,6   ),BMASS ,NHEL(6   ),+1,W(1,6   ))       
      CALL VXXXXX(P(0,7   ),WMASS ,NHEL(7   ),+1,W(1,7   ))       
      CALL JIOXXX(W(1,1   ),W(1,2   ),GG ,ZERO    ,ZERO    ,W(1,8   ))     
      CALL JIOXXX(W(1,4   ),W(1,3   ),GWF ,WMASS   ,WWIDTH  ,W(1,9   ))    
      CALL FVIXXX(W(1,5   ),W(1,9   ),GWF ,TMASS   ,TWIDTH  ,W(1,10  ))    
      CALL FVIXXX(W(1,10  ),W(1,8   ),GG ,TMASS   ,TWIDTH  ,W(1,11  ))     
      CALL IOVXXX(W(1,11  ),W(1,6   ),W(1,7   ),GWF ,AMP(1   ))            
      JAMP(   1) = -AMP(   1)
      MMYTT2 = JAMP(1)*DCONJG(JAMP(1))*2.0d0

      END


C      SUBROUTINE TOPWID(RMT,RMW,RMB,RGW,GW,RGT)
Cc*************************************************************************
Cc     THE TOTAL WEAK DECAY WIDTH OF THE TOP QUARK, INCLUDING
Cc     THE EFFECTS OF BOTTOM MASS AND, IF IGW=1,  A FINITE W WIDTH.
Cc     From James Stirling 6-10-94
Cc
Cc     RMT=TOP MASS
Cc     RMW=W   MASS
Cc     RMB=B   MASS
Cc     RGW=W   WIDTH
Cc     GW =WEAK COUPLING
Cc
Cc     RGT=TOP WIDTH
Cc
Cc*************************************************************************
C      IMPLICIT COMPLEX*16(A-H,O-Z)
C      REAL*8 RMT,RMB,RMW,XW,XB,RGW,RGT,GW
C*
C      PI=4.*DATAN(1.D0)
C      XGW=dcmplx(GW/2d0/dsqrt(2d0))
C*
C      XB=RMB/RMT
C      XW=RMW/RMT
C*
C      RM=XB**2
C      OM=1.+RM-DCMPLX(RMW**2,RMW*RGW)/RMT**2
C      Y1=OM+CDSQRT(OM*OM-4.*RM)
C      Y0=OM-CDSQRT(OM*OM-4.*RM)
C      Z1=2.
C      Z0=2.*CDSQRT(RM)
C*
C      D0=(-Y0**8+3.*Y0**7*RM+3.*Y0**7-8.*Y0**6*RM-12.*Y0**5*RM**
C     . 2-12.*Y0**5*RM+96.*Y0**4*RM**2-48.*Y0**3*RM**3-48.*Y0**3*
C     . RM**2-128.*Y0**2*RM**3+192.*Y0*RM**4+192.*Y0*RM**3-256.*
C     . RM**4)/(24.*Y0**4*(Y1-Y0))
C      D1=(-Y1**8+3.*Y1**7*RM+3.*Y1**7-8.*Y1**6*RM-12.*Y1**5*RM**
C     . 2-12.*Y1**5*RM+96.*Y1**4*RM**2-48.*Y1**3*RM**3-48.*Y1**3*
C     . RM**2-128.*Y1**2*RM**3+192.*Y1*RM**4+192.*Y1*RM**3-256.*
C     . RM**4)/(24.*Y1**4*(Y1-Y0))
C      A4=(32.*RM**4*(Y1-Y0))/(3.*Y1*Y0*(Y1-Y0))
C      A3=(8.*RM**3*(-3.*Y1**2*Y0*RM-3.*Y1**2*Y0+4.*Y1**2*RM+3.*
C     . Y1*Y0**2*RM+3.*Y1*Y0**2-4.*Y0**2*RM))/(3.*Y1**2*Y0**2*(Y1
C     . -Y0))
C      A2=(8.*RM**3*(2.*Y1**3*Y0**2-3.*Y1**3*Y0*RM-3.*Y1**3*Y0+4.
C     . *Y1**3*RM-2.*Y1**2*Y0**3+3.*Y1*Y0**3*RM+3.*Y1*Y0**3-4.*Y0
C     . **3*RM))/(3.*Y1**3*Y0**3*(Y1-Y0))
C      A1=(2.*RM**2*(3.*Y1**4*Y0**3*RM+3.*Y1**4*Y0**3+8.*Y1**4*Y0
C     . **2*RM-12.*Y1**4*Y0*RM**2-12.*Y1**4*Y0*RM+16.*Y1**4*RM**2
C     . -3.*Y1**3*Y0**4*RM-3.*Y1**3*Y0**4-8.*Y1**2*Y0**4*RM+12.*
C     . Y1*Y0**4*RM**2+12.*Y1*Y0**4*RM-16.*Y0**4*RM**2))/(3.*Y1**
C     . 4*Y0**4*(Y1-Y0))
C      B0=(Y1**3-3.*Y1**2*RM-3.*Y1**2+8.*Y1*RM-Y0**3+3.*Y0**2*RM+
C     . 3.*Y0**2-8.*Y0*RM)/(24.*(Y1-Y0))
C      B1=(Y1+Y0-3.*RM-3.)/24.
C      B2=1./24.
C*
C      RINT=D0*CDLOG((Z1-Y0)/(Z0-Y0))
C     .    -D1*CDLOG((Y1-Z1)/(Y1-Z0))
C     .    -A4/3.*(1./Z1**3-1./Z0**3)
C     .    -A3/2.*(1./Z1**2-1./Z0**2)
C     .    -A2   *(1./Z1   -1./Z0   )
C     .    +A1*CDLOG(Z1/Z0)
C     .    +B0   *(Z1   -Z0   )
C     .    +B1/2.*(Z1**2-Z0**2)
C     .    +B2/3.*(Z1**3-Z0**3)
C*
C      XGW4=XGW**4
C*
C* TOTAL WIDTH INCLUDES FLAVOUR & COLOUR FACTORS
C      RGT=RMT**3/(RMW*RGW)*XGW4/(8.*PI**3)*DIMAG(RINT)
C      RGT=9.*RGT
C      RETURN
C      END