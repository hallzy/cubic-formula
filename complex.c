#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

// Get the absolute value of a number
#define Abs(x)    ((x) < 0 ? -(x) : (x))

// Get the max of 2 numbers
#define Max(a, b) ((a) > (b) ? (a) : (b))

// Zero is the tolerance set for comparing floats. This is effectively 0 for
// comparisons
#define ZERO 0.01

// Cos is adjacent/hypotenuse. That equals -1 for -1/1 where our angle is π
#define PI acos(-1.0)

// Compare complex numbers x and y to see if they are the same
#define complexIsEqual(x, y) \
  (isEqual(x.r, y.r) && isEqual(x.i, y.i))

// Check if the real part of c is 0
#define REAL_IS_ZERO(c) (isEqual(c.r, 0))

// Check if the imaginary part of c is zero
#define IMAGINARY_IS_ZERO(c) (isEqual(c.i, 0))

// Check if complex number c is zero
#define IS_ZERO(c) (REAL_IS_ZERO(c) && IMAGINARY_IS_ZERO(c))

// square root and cbrt shortcuts
#define sqrtc(c) root(c, 2)
#define cbrtc(c) root(c, 3)

// Complex numbers are a collection of real and imaginary parts
typedef struct {
  float r;
  float i;
} complex_t;

typedef enum { false, true } bool;

// Check if two floats are effectively equivalent
bool isEqual(float a, float b) {
  // Find the greater float
  float c = Max(Abs(a), Abs(b));

  // Check if c is effectively zero
  bool closeToZero = c <= ZERO && c >= -ZERO;

  // if c was 0, then d is zero, otherwise take the difference of our two inputs
  // and divide by our max input
  float d = closeToZero ? 0.0 : Abs(a - b) / c;

  // If d is less than our Zero tolerance then a and b are very close together
  // and can be considered equivalent
  return d <= ZERO;
}


// Helper to print a complex number (prints a complex number without new line)
void print_complex_helper(complex_t c) {
  // If the complex number is 0 then just show 0
  if (IS_ZERO(c)) {
    printf("0");
    return;
  }

  // If real is zero but imaginary isn't then only show the imaginary part
  if (REAL_IS_ZERO(c) && !IMAGINARY_IS_ZERO(c)) {
    printf("%fi", c.i);
    return;
  }

  // If real isn't zero but imaginary is then only show the real part
  if (!REAL_IS_ZERO(c) && IMAGINARY_IS_ZERO(c)) {
    printf("%f", c.r);
    return;
  }

  // real and imaginary parts if we get here

  // if imaginary is negative, use a negative sign instead of a plus to combine
  // the real and imaginary
  if (c.i < 0) {
    printf("%f - %fi", c.r, (-1)*c.i);
  // Otherwise, just use the +
  } else {
    printf("%f + %fi", c.r, c.i);
  }
}

// Print a complex number
void print_complex(complex_t c) {
  // Print the number and a newline
  print_complex_helper(c);
  printf("\n");
}

// Print out an array of complex numbers of size 'size'
void print_complex_arr(complex_t *c, uint8_t size) {
  if (c == NULL) {
    return;
  }

  for (uint8_t i = 0; i < size; i++) {
    print_complex_helper(c[i]);
    printf("\n");
  }

  // Need to free c now as we aren't using this result anymore
  free(c);
}

// Create a new complex number
complex_t create(float r, float i) {
  complex_t ret = { r, i };
  return ret;
}

// Add 2 complex numbers together
complex_t add(complex_t c1, complex_t c2) {
  // Just add the real parts together, and the imaginary parts together
  complex_t ret = {
    c1.r + c2.r,
    c1.i + c2.i,
  };

  return ret;
}

// Subtract two complex numbers
complex_t sub(complex_t c1, complex_t c2) {
  // Just subtract the reals and the imaginaries
  complex_t ret = {
    c1.r - c2.r,
    c1.i - c2.i,
  };

  return ret;
}

// Multiply two complex numbers
complex_t mul(complex_t c1, complex_t c2) {
  // Need to FOIL these complex numbers

  // (F)irst
  float f = c1.r * c2.r;
  // (O)uter
  float o = c1.r * c2.i;
  // (I)nner
  float i = c1.i * c2.r;
  // (L)last
  float l = c1.i * c2.i;

  complex_t ret = {
    // First is real, last is real with an i^2. i^2 is -1 so we subtract
    f - l,
    // Outer and inner are imaginary. Add  them together
    o + i,
  };

  return ret;
}

// divide two complex numbers
complex_t divc(complex_t c1, complex_t c2) {
  // Need the conjugate of the denominator. Just negate the imaginary part
  complex_t conjugate = create(c2.r, -c2.i);

  // multiply numerator
  complex_t numerator = mul(c1, conjugate);

  // Multiple denominator by the conjugate
  // Denominator is always a real number and can be calculated like this
  // Proof:
  //   let denominator z = (x + yi) : x,y ∈ ℝ and z ∈ ℂ
  //   By definition, the conjugate of z conj(z) = (x - yi)
  //   z*conj(z) = (x + yi)(x - yi) = x^2 - yxi + xyi - y^2*i^2
  //                                = x^2 - y^2*(-1)
  //                                = x^2 + y^2
  float denominator = pow(c2.r, 2) + pow(c2.i, 2);

  // We now have a complex numerator divided by a real number
  //   ex: (x + yi)/z, which is equivalent to x/z + yi/z
  complex_t ret = {
    numerator.r/denominator,
    numerator.i/denominator,
  };

  return ret;
}

// Raise a complex number to an exponent
complex_t powc(complex_t c, uint8_t e) {
  // Start with complex number equalling 1
  complex_t ret = create(1,0);

  // If exponent is 0, then the result is 1
  if (e == 0) {
    return ret;
  }

  // If exponent is less than 0 then this is the same as 1 divided by our power
  // with positive exponent
  if (e < 0) {
    return divc(ret, powc(c, -e));
  }

  // Loop through the exponent, multiplying the result to itself each time
  for (uint8_t i = 0; i < e; i++) {
    ret = mul(ret, c);
  }

  return ret;
}

// Find the result of a radical with power e
complex_t* root(complex_t c, uint8_t e) {
  // Our root will have e solutions, so create an array of complex numbers of
  // that size
  complex_t* ret = malloc(e*sizeof(complex_t));
  if (ret == NULL) {
    return NULL;
  }

  // Zero out the return
  for (uint8_t i = 0; i < e; i++) {
    ret[i].r = 0;
    ret[i].i = 0;
  }

  // If our root is less than power of 2 then just ignore it. Return all zeroes
  if (e < 2) {
    return ret;
  }

  // Get magnitude of complex vector (Pythagoras)
  // reals is the x axis, imaginary is y axis
  float r = sqrt(pow(c.r, 2) + pow(c.i, 2));

  // Get the angle of our complex vector
  // Tan is opposite/adjacent, so y/x (or imaginary/real)
  float theta = atan2(c.i, c.r);

  // Get the root of the magnitude
  float magRoot = pow(r, 1.0/e);

  // Wee need 1 angle per solution
  for (uint8_t i = 0; i < e; i++) {
    // New angle is just our original angle multiplied by 360° (2π rad)
    // multiplied by our iteration. This gives us e equally spaced angles in our
    // plane
    float y = (theta+(2*PI*i))/e;

    // Calculate the real and imaginary parts
    ret[i].r = magRoot*cos(y);
    ret[i].i = magRoot*sin(y);
  }

  return ret;
}

// Calculate the roots of a quadratic
complex_t* quadraticFormula(float a, float b, float c) {
  // Quadratic formula is:
  //
  //  -b +/- sqrt(b^2 - 4ac)
  //  ----------------------
  //           2a

  // Calculate the inside of the square root : b^2 - 4ac
  complex_t insideSqrt = create(pow(b, 2) - 4*a*c, 0);

  // Calculate the square root
  // This gives us an array of both solutions, with automatically takes care of
  // the +/- in the equation
  complex_t *sqrtRes = sqrtc(insideSqrt);
  if (sqrtRes == NULL) {
    return NULL;
  }

  // We will get 2 solutions, so create array of size 2
  complex_t* ret = malloc(2*sizeof(complex_t));
  if (ret == NULL) {
    return NULL;
  }

  // Loop through the results of the square root to complete the formula
  for (uint8_t i = 0; i < 2; i++) {
    ret[i] = divc(add(create(-b, 0), sqrtRes[i]), create(2*a, 0));
  }

  free(sqrtRes);

  return ret;
}

// Get the roots of a cubic function
complex_t* cubicFormula(float a, float b, float c, float d) {
  // Cubic formula is as follows
  //
  //          -b^3      bc     d
  // let q = ------- + ---- + ----
  //          27a^3    6a^2    2a
  //
  //          c      b^2
  // let r = ---- - ----
  //          3a    9a^2
  //
  //
  // formula = cbrt(q + sqrt(q^2 + r^3)) + cbrt(q - sqrt(q^2 + r^3)) -  b/(3a)
  //

  // Since there are 2 cuberoots, we will get 9 different complex numbers which
  // we will need to combine into an actual solution later
  complex_t* ret = malloc(9*sizeof(complex_t));
  if (ret == NULL) {
    return NULL;
  }

  // Calculate q
  float q = (-pow(b, 3))/(27*pow(a, 3));
  q += (b*c)/(6*pow(a, 2));
  q -= (d)/(2*a);
  complex_t q_c = create(q, 0);

  // q^2 is also used, to may as well prepare that
  complex_t qSquared_c = powc(q_c, 2);

  // calculate r
  float r = ((c)/(3*a));
  r -= ((pow(b, 2))/(9*pow(a, 2)));
  complex_t r_c = create(r, 0);

  // Also calculate r^3 for later
  complex_t rCubed_c = powc(r_c, 3);

  // Calculate the square roots in the cubic formula.
  // This gives us back 2 solutions, which handles the + sqrt and the - sqrt for
  // us
  complex_t* sqrtRes = sqrtc(add(qSquared_c, rCubed_c));
  if (sqrtRes == NULL) {
    return ret;
  }

  // Calculate the result of each of the 2 cube roots
  complex_t* cbrt1 = cbrtc(add(q_c, sqrtRes[0]));
  if (cbrt1 == NULL) {
    return ret;
  }
  complex_t* cbrt2 = cbrtc(add(q_c, sqrtRes[1]));
  if (cbrt2 == NULL) {
    return ret;
  }

  // no longer need the square root results
  free(sqrtRes);

  // Common end of cubic formula
  complex_t end = create(b/(3*a), 0);

  uint8_t idx = 0;

  // Loop through all 9 combinations of cube root results to find the
  // combinations that give us actual solutions
  for (uint8_t i = 0; i < 3; i++) {
    for (uint8_t k = 0; k < 3; k++) {
      bool breakout = false;

      // Calculate the formula for one possible combination of cube root results
      complex_t tmp = sub(add(cbrt1[i], cbrt2[k]), end);

      // Does this answer already exist in our list of solutions?
      for (uint8_t x = 0; x < idx; x++) {
        if (complexIsEqual(tmp, ret[x])) {
          // It does exist, so skip to the next combination
          breakout = true;
          break;
        }
      }

      if (breakout) {
        continue;
      }

      // Check to see if tmp is a solution of the formula
      // cubic function is in form ax^3 + bx^2 + cx + d = 0
      // where a,b,c,d are provided as arguments to this function, and x is the
      // solution we just found.
      // Plug in our x to see if it comes out to 0
      complex_t val = mul(create(a, 0), powc(tmp, 3));
      val = add(val, mul(create(b, 0), powc(tmp, 2)));
      val = add(val, mul(create(c, 0), tmp));
      val = add(val, create(d, 0));

      // If it is 0 then it is a valid solution, so keep it
      if (IS_ZERO(val)) {
        ret[idx] = tmp;
        idx++;
      }
    }
  }

  free(cbrt1);
  free(cbrt2);

  return ret;
}

int main(int argc, char **argv) {
  // If no arg then we use random data for cubic
  // if 3 args, then quadtratic
  // if 4 args, then cubic
  if (argc != 1 && (argc < 4 || argc > 5)) {
    printf("invalid. Expected 3 args for quadtratic, or 4 args for cubic\n");
    return 1;
  }

  float args[4];

  // If no args, then generate random args
  if (argc == 1) {
    srand(time(NULL));

    args[0] = rand() % 50;
    args[1] = rand() % 50;
    args[2] = rand() % 50;
    args[3] = rand() % 50;

    printf("Args: %f, %f, %f, %f\n", args[0], args[1], args[2], args[3]);
  // If we did get args, then we either have 3 or 4. Add them to our args array
  // and convert to float
  } else {
    for (uint8_t i = 0; i < argc-1; i++) {
      args[i] = atof(argv[i+1]);
    }
  }

  switch (argc) {
    // If argc is 4 (3 args) then quadratic
    case 4:
      print_complex_arr(quadraticFormula(args[0], args[1], args[2]), 2);
      break;
    // If argc is 5 (4 args) then cubic. Or if no args, also cubic
    case 5:
    case 1:
      print_complex_arr(cubicFormula(args[0], args[1], args[2], args[3]), 3);
      break;
    // Shouldn't get here, but this is an error
    default:
      return 1;
  }
  return 0;
}
