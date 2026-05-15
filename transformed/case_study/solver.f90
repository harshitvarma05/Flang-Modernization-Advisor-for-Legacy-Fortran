! Small legacy solver-style code used as the case study.
      PROGRAM SOLVER
      INTEGER MODE, N
      REAL X(20), Y(20)
      COMMON /GRID/ N, X, Y
      MODE = 1
      IF (MODE) 10, 20, 30
10 CALL INIT(X, Y, N)
      GO TO 40
20 CALL STEP(X, Y, N)
      GO TO 40
30 CALL FINALIZE(X, Y, N)
40 STOP
      END

      SUBROUTINE INIT(X, Y, N)
      INTEGER N
      REAL X(*), Y(*)
      COMMON /GRID/ NG, GX, GY
      DO 100 I = 1, N
        X(I) = 0.0
        Y(I) = 0.0
100 CONTINUE
      RETURN
      END
