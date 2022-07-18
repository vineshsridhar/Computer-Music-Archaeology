C	PROGRAM FREE STOCHASTIC MUSIC (FORTRAN IV)
C
C	GLOSSARY OF THE PRINCIPAL ABBREVIATIONS
C
C	A - DURATION OF EACH SEQUENCE IN SECONDS
C	A10,A20,A17,A35,A30 - NUMBERS FOR GLISSANDO CALCULATION
C	ALEA - PARAMETER USED TO ALTER THE RESULT OF A SECOND RUN WITH THE
C	SAME INPUT DATA
C	ALFA(3) - THREE EXPRESSIONS ENTERING INTO THE THREE SPEED VALUES
C	OF THE SLIDING TONES ( GLISSANDI )
C	ALIM - MAXIMUM LIMIT OF SEQUENCE DURATION A
C	(AMAX(I),I=1,KTR) TABLE OF AN EXPRESSION ENTERING INTO THE
C	CALCULATION OF THE NOTE LENGTH IN PART 8
C	BF - DYNAMIC FORM NUMBER. THE LIST IS ESTABLISHED INDEPENDENTLY
C	OF THIS PROGRAM AND IS SUBJECT TO MODIFICATION
C	DELTA - THE RECIPROCAL OF THE MEAN DENSITY OF SOUND EVENTS DURING
C	A SEQUENCE OF DURATION A
C	(E(I,J),I=1,KTR,J=1,KTE) - PROBABILITIES OF THE KTR TIMBRE CLASSES
C	INTRODUCED AS INPUT DATA, DEPENDING ON THE CLASS NUMBER I=KR AND
C	ON THE POWER J=U OBTAINED FROM V3*EXPF(U)=DA
C	EPSI - EPSILON FOR ACCURACY IN CALCULATING PN AND E(I,J), WHICH
C	IT IS ADVISABLE TO RETAIN.
C	(GN(I,J),I=1,KTR,J=1,KTS) - TABLE OF THE GIVEN LENGTH OF BREATH
C	FOR EACH INSTRUMENT, DEPENDING ON CLASS I AND INSTRUMENT J
C	GTNA - GREATEST NUMBER OF NOTES IN THE SEQUENCE OF DURATION A
C	GTNS - GREATEST NUMBER OF NOTES IN KW LOOPS
C	(HAMIN(I,J),HAMAX(I,J),HBMIN(I,J),HBMAX(I,J),I=1,KTR,J=1,KTS)
C	TABLE OF INSTRUMENT COMPASS LIMITS, DEPENDING ON TIMBRE CLASS I
C	AND INSTRUMENT J. TEST INSTRUCTION 480 IN PART 6 DETERMINES
C	WHETHER THE HA OR THE HB TABLE IS FOLLOWED. THE NUMBER 7 IS
C	ARBITRARY.
C	JW - ORDINAL NUMBER OF THE SEQUENCE COMPUTED.
C	KNL - NUMBER OF LINES PER PAGE OF THE PRINTED RESULT.KNL=50
C	KR1 - NUMBER IN THE CLASS KR=1 USED FOR PERCUSSION OR INSTRUMENTS
C	WITHOUT A DEFINITE PITCH.
C	KTE - POWER OF THE EXPONENTIAL COEFFICIENT E SUCH THAT
C	DA(MAX)=V3*(E**(KTE-1))
C	KTR - NUMBER OF TIMBRE CLASSES
C	KW - MAXIMUM NUMBER OF JW
C	KTEST1,TAV1,ETC - EXPRESSIONS USEFUL IN CALCULATING HOW LONG THE
C	VARIOUS PARTS OF THE PROGRAM WILL RUN.
C	KT1 - ZERO IF THE PROGRAM IS BEING RUN, NONZERO DURING DEBUGGING
C	KT2 - NUMBER OF LOOPS, EQUAL TO 15 BY ARBITRARY DEFINITION.
C	(MODI(IX8),IX8=7,1) AUXILIARY FUNCTION TO INTERPOLATE VALUES IN
C	THE TETA(256) TABLE (SEE PART 7)
C	NA - NUMBER OF SOUNDS CALCULATED FOR THE SEQUENCE A(NA=DA*A)
C	(NT(I),I=1,KTR) NUMBER OF INSTRUMENTS ALLOCATED TO EACH OF THE
C	KTR TIMBRE CLASSES.
C	(PN(I,J),I=1,KTR,J=1,KTS),(KTS=NT(I),I=1,KTR) TABLE OF PROBABILITY
C	OF EACH INSTRUMENT OF THE CLASS I.
C	(Q(I),I=1,KTR) PROBABILITIES OF THE KTR TIMBRE CLASSES. CONSIDERED
C	AS LINEAR FUNCTIONS OF THE DENSITY DA.
C	(S(I),I=1,KTR) SUM OF THE SUCCESS Q(I) PROBABILITIES, USED TO
C	CHOOSE THE CLASS KR BY COMPARING IT TO A RANDOM NUMBER X1 (SEE
C	PART 3, LOOP 380 AND PART 5, LOOP 430).
C	SINA - SUM OF THE COMPUTED NOTES IN THE JW CLOUDS NA, ALWAYS LESS
C	THAN GTNS ( SEE TEST IN PART 10 ).
C	SQPI - SQUARE ROOT OF PI (3.14159...)
C	TA - SOUND ATTACK TIME ABCISSA.
C	TETA(256) - TABLE OF THE 256 VALUES OF THE INTEGRAL OF THE NORMAL
C	DISTRIBUTION CURVE WHICH IS USEFUL IN CALCULATING GLISSANDO SPEED
C	AND SOUND EVENT DURATION.
C	VIGL - GLISSANDO SPEED (VITESSE GLISSANDO), WHICH CAN VARY AS, BE
C	INDEPENDENT OF, OR VARY INVERSELY AS THE DENSITY OF THE SEQUENCE,
C	THE ACTUAL MODE OF VARIATION EMPLOYED REMAINING THE SAME FOR THE
C	ENTIRE SEQUENCE (SEE PART 7).
C	VITLIM - MAXIMUM LIMITING GLISSANDO SPEED (IN SEMITONES/SEC),
C	SUBJECT TO MODIFICATION.
C	V3 - MINIMUM CLOUD DENSITY DA
C	(Z1(I),Z2(I),I=1,8) TABLE COMPLEMENTARY TO THE TETA TABLE
C	
C
C	READ CONSTANTS AND TABLES
C
	DIMENSION Q(12),S(12),E(12,12),PN(12,50),SPN(12,50),NT(12),
	*HAMIN(12,50),HAMAX(12,50),HBMIN(12,50),HBMAX(12,50),GN(12,50),H(12
	*,50),TETA(256),VIGL(3),MODI(7),Z1(8),Z2(8),ALFA(3),AMAX(12)
C
C
C
	I=1
	DO 10 IX=1,7
	IX8=8-IX
	MODI(IX8)=I
10	I=I+1
C
	READ 20,(TETA(I),I=1,256)
20	FORMAT(12F6.6)
	READ 30,(Z1(I),Z2(I),I=1,8)
30	FORMAT(6(F3.2,F9.8)/F3.2,F9.8,E6.2,F9.8)
	PRINT 40,TETA,Z1,Z2
40	FORMAT(*1  THE TETA TABLE = *,/,21(12F10.6,/),4F10.6,/////,
	** THE Z1 TABLE = *,/,7F6.2,E12.3,///,* THE Z2 TABLE = *,/,8F14.8,/
	*,1H1)
	READ 50,DELTA,V3,A10,A20,A17,A30,A35,BF,SQPI,EPSI,VITLIM,ALEA,
	*LIM
50	FORMAT(F3.0,F3.3,5F3.1,F2.0,F8.7,F8.8,F4.2,F8.8,F5.2)
	READ 60,KT1,KT2,KW,KNL,KTR,KTE,KR1,GTNA,GTNS,(NT(I),I=1,KTR)
60	FORMAT(5I3,2I2,2F6.0,I2I2)
	PRINT 70,DELTA,V3,A10,A20,A17,A30,A35,BF,SQPI,EPSI,VITLIM,ALEA,
	*LIM,KT1,KT2,KW,KNL,KTR,KTE,KR1,GTNA,GTNS,((I,NT(I)),I=1,KTR)
70	FORMAT(*1DELTA = *,F4.0,/,* V3 = *,F6.3,/,* A10 = *,F4.1,/,
	** A20 = *,F4.1,/,* A17 = *,F4.1,/,* A30 = *,F4.1,/,* A35 = *,F4.1,
	*/,* BF = *,F3.0,/,* SQP1 =*,F11.8,/,* EPSI =*,F12.8,/,* VITLIM = *
	*,F5.2,/,* ALEA =*,F12.8,/,* ALIM = *,F6.2,/,* KT1 = *,I3,/,
	** KT2 = *,I3,/,* KW = *,I3,/,* KNL = *,I3,/,* KTR = *,I3,/,
	** KTE = *,I2,/,* KR1 = *,I2,/,* GTNA = *,F7.0,/,* GTNS = *,F7.0,
	*/,I2(* IN CLASS *,I2,*, THERE ARE *,I2,* INSTRUMENTS.*,/))
	READ 80,KTEST3,KTEST1,KTEST2
80	FORMAT(5I3)
	PRINT 90,KTEST3,KTEST1,KTEST2
90	FORMAT(* KTEST3 = *,I3,/,* KTEST1 = *,I3,/,* KTEST2 = *,I3)
C
	IF(KTEST3.NE.0) PRINT 830
	R=KTE-1
	A10=A10*SQPI
	A20=A20*SQPI/R
	A30=A30*SQPI
C	IF ALEA IS NON-ZERO, THE RANDOM NUMBER IS GENERATED FROM THE TIME
C	WHEN THE FOLLOWING INSTRUCTION IS EXECUTED. IF ALEA IS NON-ZERO
C	EACH RUN OF THIS PROGRAM WILL PRODUCE DIFFERENT OUTPUT DATA.
	IF(ALEA.NE.0.0) CALL RANFSET(TIMEF(1))
	PRINT 830
	DO 130 I=1,KTR
	Y=0.0
	KTS=NT(I)
	READ 100,(HAMIN(I,J),HAMAX(I,J),HBMIN(I,J),HBMAX(I,J),GN(I,J),
	*PN(I,J),J=1,KTS)
100	FORMAT (5(5F2.0,F3.3))
	PRINT 110,I,(J,HAMIN(I,J),HAMAX(I,J),HBMIN(I,J),HBMAX(I,J),GN(I,J)
	*,PN(I,J),J=1,KTS)
110	FORMAT(////,* IN CLASS NUMBER *,I2,(/,* FOR INSTRUMENT NO. *,I2,
	** HAMIN = *,F3.0,*,HAMAX = *,F3.0,*,HBMIN = *,F3.0,*,HBMAX = *,
	* F3.0,*,GN = *,F3.0,*, AND PN = *,F6.3))
	DO 120 J=1,KTS
	Y=Y+PN(I,J)
120	SPN(I,J)=Y
130	IF (ABSF(Y-1.0).GE.EPSI) CALL EXIT
C
	DO 150 I=1,KTR
	READ 140,(E(I,J),J=1,KTE)
140	FORMAT(12F2.2)
150	PRINT 160,I,(J,E(I,J),J=1,KTE)
160	FORMAT(//////,* CLASS NUMBER *,I2,/,(* IN DENSITY LEVEL *,I2,
	** HAS A PROBABILITY OF *,F6.2,/))
	DO 180 J=1,KTE
	Y=0.0
	DO 170 I=1,KTR
170	Y=Y+E(I,J)
180	IF (ABSF(Y-1.0).GE.EPSI) CALL EXIT
	DO 200 I=1,KTR
	AMAX(I) = 1.0/E(I,1)
	DO 200 J=2,KTE
	AJ=J-1
	AX=1.0/(E(I,J)*EXPF(AJ))
	IF(KT1.NE.0) PRINT 190,AX
190	FORMAT(1H ,9E12.8)
200	IF (AX.GT.AMAX(I)) AMAX(I)=AX
	IF(KT1.NE.0) PRINT 210,AMAX
210	FORMAT( 1H ,9E12.8)
C
	JW=1
	SINA=0.0
	IF (KTEST1.NE.0) TAV1=TIMEF(1)
220	NLINE=50
C
C	PARTS 1 AND 2, DEFINE SEQUENCE A SECONDS AND CLOUD NA DURING A
C
	KNA=0
	K1=0
230	X1=RNAF(-1)
	A=-DELTA * LOGF(X1)
	IF(A.LE.ALIM) GO TO 250
	IF (K1.GE.KT2) GO TO 240
	K1=K1+1
	GO TO 230
240	A=ALIM/2.0
	X1=0.0
250	K2=0
260	X2=RANF(-1)
	IF (JW.GT.1) GO TO 280
270	UX=R*X2
	GO TO 310
280	IF RANF(-1).GE.0.5) GO TO 290
	UX=UPR + R * (1.0-SQRTF(X2))
	GO TO 300
290	UX=UPR - R * ( 1.0-SQRTF(X2))
300	IF ((UX.GE.0.0).AND.(UX.LE.R)) GO TO 310
	IF (K2.GE.KT2) GO TO 270
	K2=K2+1
	GO TO 260
310	U=UX
	DA=V3 * EXPF(U)
	NA=XINTF(A * DA + 0.5) + 1
	IF (GTNA.GT.FLOATF(NA)) GO TO 330
	IF (KNA.GE.KT2) GO TO 320
	KNA=KNA+1
	GO TO 230
320	A=DELTA
	GO TO 260
330	UPR=U
	IF(KT1.EQ.0) GO TO 360
	PRINT 340,JW,KNA,K1,K2,X1,X2,A,DA,NA
340	FORMAT(1H1,4I8,3X,4E18.8,3X,I8)
	NA=KT1
	IF (KTEST3.NE.0) PRINT 350,JW,NA,A
350	FORMAT(1H0,2I9,F10.2)
C
C	PART 3, DEFINE CONSTITUTION OF ORCHESTRA DURING SEQUENCE A
C
360	SINA=SINA + FLOATF(NA)
	XLOGDA=U
	XALOG=A20 *XLOGDA
	M=XINTF(XLOGDA)
	IF ((M+2).GT.KTE) M=KTE-2
	SR=0.0
	M1=M+1
	M2=M+2
	DO 380 I=1,KTR
	ALFX=E(I,M1)
	BETA=E(I,M2)
	XM=M
	QR=(XLOGDA-XM) * (BETA-ALFX) + ALFX
	IF (KT1.NE.0) PRINT 370,XM,ALFX,BETA
370	FORMAT(1H ,3F20.8)
	Q(I)=QR
	SR=SR+QR
380	S(I)=SR
	IF (KT1.NE.0) PRINT 390,(Q(I),I=1,KTR,(S(I),I=1,KTR)
390	FORMAT(1H ,12F9.4)
C
C	PART 4, DEFINE INSTANT TA OF EACH POINT IN SEQUENCE A
C
	IF (KTEST2.NE.0) TAV2=TIMEF(1)
	N=1
	T=0.0
	TA=0.0
	GO TO 410
400	N=N+1
	X=RANF(-1)
	T=-LOGF(X)/DA
	TA=TA+T
410	IF (KT1.NE.0) PRINT 420,N,X,T,TA
420	FORMAT(//,I8,3E20.8)
C
C	PART 5,DEFINE CLASS AND INSTRUMENT NUMBER TO EACH POINT OF A
C
	X1=RANF(-1)
	DO 430 I=1,KTR
430	IF (X1.LE.S(I)) GO TO 440
	I=KTR
440	KTS=NT(I)
	KR=I
	X2=RANF(-1)
	DO 450 J=1,KTS
	SPIEN=SPN(KR,J)
	INSTRM=J
450	IF (X2.LE.SPIEN) GO TO 460
	INSTRM=KTS
460	PIEN=PN(KR,INSTRM)
	IF (KT1.NE.0) PRINT 470,X1,S(KR),KR,X2,SPIEN,INSTRM
470	FORMAT( 1H ,2E20.8,I6,2E20.8,I6 )
C
C	PART 6,DEFINE PITCH HN FOR EACH POINT OF SEQUENCE A
C
	IF (KR.GT.1) GO TO 480
	IF (INSTRM.GE.KR1) GO TO 490
	HX=0.0
	GO TO 560
480	IF (KR.LT.7) GO TO 490
	HSUP=HBMAX(KR,INSTRM)
	HINF=HBMIN(KR,INSTRM)
	GO TO 500
490	HSUP=HAMAX(KR,INSTRM)
	HINF=HAMIN(KR,INSTRM)
500	HM=HSUP-HINF
	HPR=H(KR,INSTRM)
	K=0
C	IF (HPR.LE.0.0) GO TO 520
C510	X=RANF(-1)
C Bug noted by Rogers in his "User's Manual"
C Swapping the statements ensures that X is initialized before it is used. 
510	X=RANF(-1)
	IF (HPR.LE.0.0) GO TO 520
	IF (N.GT.1) GO TO 530
520	HX=HINF+HM*X RANF(-1)
	GO TO 560
530 	IF (RANF(-1).GE.0.5) GO TO 540
	HX=HPR+HM * (1.0-SQRTF(X))
	GO TO 550
540	HX=HPR-HM * (1.0-SQRTF(X))
550	IF((HX.GE.HINF).AND.(HX.LE.HSUP)) GO TO 560
	IF (K.GE.KT2) GO TO 520
	K=K+1
	GO TO 510
560	H(KR,INSTRM)=HX
	IF (KT1.NE.0) PRINT 570,K,X,HX
570	FORMAT(1H ,I6,2E20.8)
C
C	PART 7,DEFINE SPEED VIGL TO EACH POINT OF A
C
	IF (KR.EQ.5) GO TO 580
	VIGL(1)=0.0
	VIGL(2)=0.0
	VIGL(3)=0.0
	X1=0.0
	X2=0.0
	XLAMBDA=0.0
	GO TO 740
580	KX=1
590	X1=RANF(-1)
	IF (X1-0.9997) 600,650,680
600	I=128
	DO 630 IX=1,7
	IF(TETA(I)-X1) 610,640,620
610	I=I+MODI(IX)
	GO TO 630
620	I=I-MODI(IX)
630	CONTINUE
	IF(TETA(I)-X1) 670,640,660
640	XLAMBDA=FLOATF(I-1)/100.0
	GO TO (720,760), KX
650	XLAMBDA=2.55
	GO TO (720,760),KX
660	I=I-1
670	TX1=TETA(I)
	XLAMBDA=(FLOATF(I-1)+(X1-TX1)/(TETA(I+1)-TX1))/100.0
	GO TO ( 720,760 ), KX
680	DO 690 I=2,7
	TX1=Z2(I)
	IF(X1-TX1) 700,710,690
690	CONTINUE
	I=8
	TX1=1.0
700	TX2=Z1(I)
	XLAMBDA=TX2-((TX1-X1)/(TX1-Z2(I-1)))*(TX2-Z1(I-1))
	GO TO ( 720,760 ), KX
710	XLAMBDA=Z1(I)
	GO TO ( 720,760 ), KX
720	ALFA(1)=A10+XALOG
	ALFA(3)=A30-XALOG
	X2=RANF(-1)
	ALFA(2)=A17+A35*X2
	DO 730 I=1,3
	VIGL(I)=INTF(ALFA(I)*XLAMBDA+0.5)
	IF (VIGL(I).LT.0.0) VIGL(I)=-VIGL(I)
	IF (VIGL(I).GT.VITLIM) VIGL(I)=VITLIM
730	IF (RANF(-1).LT.0.5) VIGL(I)=-VIGL(I)
740	IF(KT1.NE.0) PRINT 750,X1,X2,XLAMBDA,VIGL
750	FORMAT(1H ,6E19.8)
C
C	PART 8,DEFINE DURATION FOR EACH POINT OF A
C
	IF ((KR.EQ.7).OR.(KR.EQ.8)) GO TO 780
	ZMAX=AMAX(KR)/(V3*PIEN)
	G=GN(KR,INSTRM)
	RO=G/LOGF(ZMAX)
	QPNDA=1.0/(Q(KR)*PIEN*DA)
	GE=ABSF(RO*LOGF(QPNDA))
	XMU=GE/2.0
	SIGMA=GE/4.0
	KX=2
	GO TO 590
760	TAU=SIGMA*XLAMBDA*1.4142
	X2=RANF(-1)
	IF (X2.GE.0.5) GO TO 770
	XDUR=XMU+TAU
	GO TO 790
770	XDUR=XMU-TAU
	IF (XDUR.GE.0.0) GO TO 790
780	XDUR = 0.0
790	IF(KT1.NE.0)PRINT 800,ZMAX,XMU,SIGMA,X1,XLAMBDA,X2,XDUR
800	FORMAT(1H ,5E15.8,E11.4,E15.8)
C
C	PART 9,DEFINE INTENSITY FORM TO EACH POINT OF A
C
	IFORM=XINTF(RANF(-1)*BF+0.5)
	IF (KT1.EQ.0) GO TO 840
	IF (NLINE.LT.KNL) GO TO 810
	IF (NLINE.EQ.KNL) GO TO 820
	NLINE=1
	GO TO 900
810	NLINE=NLINE+1
	GO TO 900
820	PRINT 830
830	FORMAT(1H1)
	NLINE=0
	GO TO 900
840	IF (NLINE.GE.KNL) GO TO 850
	NLINE=NLINE+1
	GO TO 880
850	PRINT 860,JW,A,NA,(Q(I),I=1,KTR)
860	FORMAT(*1  JW=*,I3,4X,*A=*,F8.2,4X,*NA=*,I6,4X,*Q(I)=*,I2(F4.2,*/*
	*),//)
	PRINT 870
870	FORMAT(6X,*N*,8X,*START*,5X,*CLASS*,4X,*INSTRM*,4X,*PITCH*,6X,
	**GLISS1*,4X,*GLISS2*,4X,*GLISS3*,8X,*DURATION*,5X,*DYNAM*)
	NLINE=1
880	PRINT 890,N,TA,KR,INSTRM,HX,(VIGL(I),I=1,3),XDUR,IFORM
890	FORMAT(1H ,I7,F12.2,I9,I8,F11.1,F13.1,2F10.1,F14.2,I11)
C
C	PART 10,REPEAT SAME DEFINITIONS FOR ALL POINTS OF A
C
900	IF (N.LT.NA) GO TO 400
C
C	PART 11, REPEAT SEQUENCE A
C
	IF (KTEST2.EQ.0) GO TO 910
	TAP2=TIMEF(1)-TAV2
	TAP2=TAP2/FLOATF(NA)
	PRINT 750,TAP2
C
910	IF (JW.GE.KW) GO TO 930
920	JW=JW+1
	IF (GTNS.GT.SINA) GO TO 220
930	IF (KTEST1.EQ.0) CALL EXIT
940	TAP1=TIMEF(-1)-TAV1
	TAP1=TAP1/FLOATF(KW)
	PRINT 750,TAP1
C
	END