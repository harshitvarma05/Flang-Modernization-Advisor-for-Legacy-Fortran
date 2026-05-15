C Legacy fixture covering modernization advisor patterns.
      PROGRAM LEGACY
      INTEGER I, J, K, IDX
      REAL A(10), B(10), X
      COMMON /STATE/ I, J, A
      EQUIVALENCE (A(1), B(1))
      F(Y) = Y * Y + X
      IDX = 2
      IF (X) 10, 20, 30
 10   I = 1
      GO TO 40
 20   I = 2
      GO TO 40
 30   I = 3
 40   GO TO (100, 200, 300), IDX
 100  CONTINUE
      CALL WORK(A, 10)
      STOP
 200  CONTINUE
      STOP
 300  CONTINUE
      STOP
      END

      SUBROUTINE WORK(ARR, N)
      REAL ARR(*)
      INTEGER N
      COMMON /STATE/ I, J, A
      ENTRY WORK2(ARR, N)
      ARR(1) = ARR(1) + 1.0
      RETURN
      END
