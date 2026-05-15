      SUBROUTINE STEP(X, Y, N)
      INTEGER N, I
      REAL X(*), Y(*), TMP(2)
      COMMON /GRID/ NG, GX, GY
      EQUIVALENCE (TMP(1), LOCAL)
      SCALE(Z) = Z * 0.5
      DO 100 I = 1, N
        X(I) = X(I) + SCALE(Y(I))
 100  CONTINUE
      RETURN
      END

      SUBROUTINE FINALIZE(X, Y, N)
      INTEGER N
      REAL X(*), Y(*)
      COMMON /GRID/ NG, GX, GY
      RETURN
      END
