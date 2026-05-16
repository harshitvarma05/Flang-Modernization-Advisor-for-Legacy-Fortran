program minpack_driver
  implicit none
  integer, parameter :: n = 1
  integer, parameter :: lwa = (n * (3 * n + 13)) / 2
  double precision :: x(n), fvec(n), tol, wa(lwa)
  integer :: info
  external fcn

  x(1) = 2.0d0
  tol = 1.0d-8

  call hybrd1(fcn, n, x, fvec, tol, info, wa, lwa)

  print *, 'MINPACK hybrd1 demo'
  print *, 'info = ', info
  print *, 'solution x = ', x(1)
  print *, 'residual f(x) = ', fvec(1)
end program minpack_driver

subroutine fcn(n, x, fvec, iflag)
  implicit none
  integer, intent(in) :: n
  integer, intent(inout) :: iflag
  double precision, intent(in) :: x(n)
  double precision, intent(out) :: fvec(n)

  fvec(1) = x(1) * x(1) - 1.0d0
end subroutine fcn
