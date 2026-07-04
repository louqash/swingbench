# Euler–Lagrange derivation (double pendulum)

$$L = T - V$$

## Setup

Pivot at the origin. Generalized coordinates: the absolute angles $\theta_1, \theta_2$.
Screen convention: $(0,0)$ is the top-left corner, so $y$ increases downward.

```
O(x0, y0)
|\
| \
|  \ l1
|   \
     \
      O(x1, y1)  (mass = m1)
      \
      ||
      | \ l2
      |  |
         \
        O(x2, y2)  (mass = m2)
```

## Positions

$$x_1 = x_0 + l_1 \sin\theta_1 \qquad y_1 = y_0 + l_1 \cos\theta_1$$

$$x_2 = x_1 + l_2 \sin\theta_2 \qquad y_2 = y_1 + l_2 \cos\theta_2$$

## Velocities

$$\dot x_1 = l_1 \cos\theta_1 \dot\theta_1 \qquad \dot y_1 = -l_1 \sin\theta_1 \dot\theta_1$$

$$v_1 = \sqrt{\dot x_1^2 + \dot y_2^2} \qquad v_1^2 = \dot x_1^2 + \dot y_1^2$$

$$\dot x_2 = \dot x_1 + l_2 \cos\theta_2 \dot\theta_2 \qquad \dot y_2 = \dot y_1 - l_2 \sin\theta_2 \dot\theta_2$$

$$v_2 = \sqrt{\dot x_2^2 + \dot y_2^2} \qquad v_2^2 = \dot x_2^2 + \dot y_2^2$$

## Kinetic energy

$$T = \tfrac{1}{2} m v^2 \qquad\Longrightarrow\qquad T = \tfrac{1}{2} m_1 v_1^2 + \tfrac{1}{2} m_2 v_2^2$$

## Potential energy

$$V = m g h, \qquad h_1 = -y_1, \quad h_2 = -y_2$$

$$V = g (m_1 h_1 + m_2 h_2)$$
$$V = -g (m_1 y_1 + m_2 y_2)$$
## Lagrangian

$$L = T - V$$

$$L = \tfrac{1}{2} (m_1 v_1^2 + m_2 v_2^2) - g (m_1 h_1 + m_2 h_2)$$

$$L = \tfrac{1}{2} \big(m_1 (\dot x_1^2 + \dot y_1^2) + m_2 (\dot x_2^2 + \dot y_2^2)\big) - g (m_1 -y_1 + m_2 -y_2)$$

Substituting the velocities:

$$
\begin{aligned}
L &= \tfrac{1}{2} m_1 \big((\cos\theta_1 l_1 \dot\theta_1)^2 + (-\sin\theta_1 l_1 \dot\theta_1)^2\big) \\
&\quad + \tfrac{1}{2} m_2 \big((\dot x_1 + \cos\theta_2 l_2 \dot\theta_2)^2 + (\dot y_1 - \sin\theta_2 l_2 \dot\theta_2)^2\big) \\
&\quad + g (m_1 y_1 + m_2 y_2)
\end{aligned}
$$

$$
\begin{aligned}
L &= \tfrac{1}{2} m_1 \big((\cos\theta_1 l_1 \dot\theta_1)^2 + (-\sin\theta_1 l_1 \dot\theta_1)^2\big) \\
&\quad + \tfrac{1}{2} m_2 \big((\cos\theta_1 l_1 \dot\theta_1 + \cos\theta_2 l_2 \dot\theta_2)^2
+ (-\sin\theta_1 l_1 \dot\theta_1 - \sin\theta_2 l_2 \dot\theta_2)^2\big) \\
&\quad + g \big(m_1 (y_0 + \cos\theta_1 l_1) + m_2 (y_1 + \cos\theta_2 l_2)\big)
\end{aligned}
$$

Expanding the squares:

$$
\begin{aligned}
L &= \tfrac{1}{2} m_1 \big(\cos^2\theta_1 l_1^2 \dot\theta_1^2 + \sin^2\theta_1 l_1^2 \dot\theta_1^2\big) \\
&\quad + \tfrac{1}{2} m_2 \big(\cos^2\theta_1 l_1^2 \dot\theta_1^2
+ 2 \cos\theta_1 l_1 \dot\theta_1 \cos\theta_2 l_2 \dot\theta_2
+ \cos^2\theta_2 l_2^2 \dot\theta_2^2 \\
&\qquad\qquad + \sin^2\theta_1 l_1^2 \dot\theta_1^2
+ 2 \sin\theta_1 l_1 \dot\theta_1 \sin\theta_2 l_2 \dot\theta_2
+ \sin^2\theta_2 l_2^2 \dot\theta_2^2\big) \\
&\quad + g \big(m_1 (y_0 + \cos\theta_1 l_1) + m_2 (y_1 + \cos\theta_2 l_2)\big)
\end{aligned}
$$

Grouping:

$$
\begin{aligned}
L &= \tfrac{1}{2} m_1 l_1^2 \dot\theta_1^2 (\cos^2\theta_1 + \sin^2\theta_1) \\
&\quad + \tfrac{1}{2} m_2 \big(l_1^2 \dot\theta_1^2 (\cos^2\theta_1 + \sin^2\theta_1)
+ l_2^2 \dot\theta_2^2 (\cos^2\theta_2 + \sin^2\theta_2) \\
&\qquad\qquad + 2 l_1 \dot\theta_1 l_2 \dot\theta_2 (\cos\theta_1 \cos\theta_2 + \sin\theta_1 \sin\theta_2)\big) \\
&\quad + g \big(m_1 (y_0 + \cos\theta_1 l_1) + m_2 (y_1 + \cos\theta_2 l_2)\big)
\end{aligned}
$$

Using $\cos^2 + \sin^2 = 1$ and $\cos\theta_1 \cos\theta_2 + \sin\theta_1 \sin\theta_2 = \cos(\theta_1 - \theta_2)$:

$$
\begin{aligned}
L &= \tfrac{1}{2} m_1 l_1^2 \dot\theta_1^2 \\
&\quad + \tfrac{1}{2} m_2 \left(l_1^2 \dot\theta_1^2 + l_2^2 \dot\theta_2^2 + 2 l_1 l_2 \dot\theta_1 \dot\theta_2 \cos(\theta_1 - \theta_2)\right) \\
&\quad + g \big(m_1 (y_0 + l_1 \cos\theta_1) + m_2 (y_0 + l_1 \cos\theta_1 + l_2 \cos\theta_2)\big)
\end{aligned}
$$

## Euler–Lagrange

$$\frac{d}{dt}\left(\frac{\partial L}{\partial \dot q}\right) - \frac{\partial L}{\partial q} = 0$$

### For $\theta_1$

$$\frac{d}{dt}\left(\frac{\partial L}{\partial \dot\theta_1}\right) - \frac{\partial L}{\partial \theta_1} = 0$$

First $\partial L / \partial \theta_1$:

$$
\begin{aligned}
\frac{\partial L}{\partial \theta_1}
= \frac{\partial}{\partial \theta_1} \bigg[
& \tfrac{1}{2} m_1 l_1^2 \dot\theta_1^2 \\
& + \tfrac{1}{2} m_2 \left(l_1^2 \dot\theta_1^2 + l_2^2 \dot\theta_2^2 + 2 l_1 l_2 \dot\theta_1 \dot\theta_2 \cos(\theta_1 - \theta_2)\right) \\
& + g \big(m_1 (y_0 + l_1 \cos\theta_1) + m_2 (y_0 + l_1 \cos\theta_1 + l_2 \cos\theta_2)\big) \bigg]
\end{aligned}
$$

$$\frac{\partial L}{\partial \theta_1}
= \tfrac{1}{2} m_2 \big(2 l_1 l_2 \dot\theta_1 \dot\theta_2 (-\sin(\theta_1 - \theta_2))\big)
+ g(m_1 l_1 (- \sin\theta_1) + m_2 l_1 (-\sin\theta_1))$$

$$\frac{\partial L}{\partial \theta_1}
= -m_2 l_1 l_2 \dot\theta_1 \dot\theta_2 \sin(\theta_1 - \theta_2)
- g l_1 (m_1 + m_2) \sin\theta_1$$

Then $\partial L / \partial \dot\theta_1$:

$$\frac{\partial L}{\partial \dot\theta_1}
= m_1 l_1^2 \dot\theta_1 + \tfrac{1}{2} m_2 \big(2 l_1^2 \dot\theta_1 + 2 l_1 l_2 \dot\theta_2 \cos(\theta_1 - \theta_2)\big)$$

$$\frac{\partial L}{\partial \dot\theta_1}
= m_1 l_1^2 \dot\theta_1 + m_2 l_1 \big(l_1 \dot\theta_1 + l_2 \dot\theta_2 \cos(\theta_1 - \theta_2)\big)$$

$$
\begin{aligned}
\frac{d}{dt}\left(\frac{\partial L}{\partial \dot\theta_1}\right)
&= m_1 l_1^2 \ddot\theta_1 \\
&\quad + m_2 l_1 \Big(l_1 \ddot\theta_1 + l_2 \big(\ddot\theta_2 \cos(\theta_1 - \theta_2)
+ \dot\theta_2 (-\sin(\theta_1 - \theta_2)(\dot\theta_1 - \dot\theta_2))\big)\Big)
\end{aligned}
$$

Combining and simplifying:

$$
\begin{aligned}
& m_1 l_1^2 \ddot\theta_1 + m_2 l_1 \big(l_1 \ddot\theta_1 + l_2 (\ddot\theta_2 \cos(\theta_1 - \theta_2) + \dot\theta_2 (-\sin(\theta_1 - \theta_2)(\dot\theta_1 - \dot\theta_2)))\big) \\
&\quad - \big(m_2 l_1 \dot\theta_1 l_2 \dot\theta_2 (-\sin(\theta_1 - \theta_2)) + g l_1 (-\sin\theta_1)(m_1 + m_2)\big) = 0
\end{aligned}
$$

$$
\begin{aligned}
& m_1 l_1^2 \ddot\theta_1 + m_2 l_1^2 \ddot\theta_1 + m_2 l_1 l_2 \big(\ddot\theta_2 \cos(\theta_1 - \theta_2) + \dot\theta_2 (-\sin(\theta_1 - \theta_2)(\dot\theta_1 - \dot\theta_2))\big) \\
&\quad - m_2 l_1 \dot\theta_1 l_2 \dot\theta_2 (-\sin(\theta_1 - \theta_2) - g l_1 (-\sin\theta_1)(m_1 + m_2) = 0
\end{aligned}
$$

$$
\begin{aligned}
& (m_1 + m_2) l_1^2 \ddot\theta_1 + m_2 l_1 l_2 \ddot\theta_2 \cos(\theta_1 - \theta_2) \\
&\quad + m_2 l_1 l_2 \dot\theta_2 (-\sin(\theta_1 - \theta_2))(\dot\theta_1 - \dot\theta_2) \\
&\quad - m_2 l_1 l_2 \dot\theta_2 \dot\theta_1 (-\sin(\theta_1 - \theta_2)) \\
&\quad - g l_1 (-\sin\theta_1)(m_1 + m_2) = 0
\end{aligned}
$$

$$
\begin{aligned}
& (m_1 + m_2) l_1^2 \ddot\theta_1 + m_2 l_1 l_2 \ddot\theta_2 \cos(\theta_1 - \theta_2) \\
&\quad + m_2 l_1 l_2 \dot\theta_2 (-\sin(\theta_1 - \theta_2)) \big((\dot\theta_1 - \dot\theta_2) - \dot\theta_1\big) \\
&\quad - g l_1 (-\sin\theta_1)(m_1 + m_2) = 0
\end{aligned}
$$

**Euler–Lagrange equation for $\theta_1$:**

$$
\begin{aligned}
(m_1 + m_2) l_1^2 \ddot\theta_1 &+ m_2 l_1 l_2 \ddot\theta_2 \cos(\theta_1 - \theta_2) \\
&= -g l_1 (m_1 + m_2) \sin\theta_1 - m_2 l_1 l_2 \dot\theta_2^{\,2} \sin(\theta_1 - \theta_2)
\end{aligned}
$$

### For $\theta_2$

$$\frac{d}{dt}\left(\frac{\partial L}{\partial \dot\theta_2}\right) - \frac{\partial L}{\partial \theta_2} = 0$$

First $\partial L / \partial \theta_2$:

$$
\begin{aligned}
\frac{\partial L}{\partial \theta_2}
= \frac{\partial}{\partial \theta_2} \bigg[
& \tfrac{1}{2} m_1 l_1^2 \dot\theta_1^2 \\
& + \tfrac{1}{2} m_2 \left(l_1^2 \dot\theta_1^2 + l_2^2 \dot\theta_2^2 + 2 l_1 l_2 \dot\theta_1 \dot\theta_2 \cos(\theta_1 - \theta_2)\right) \\
& + g \big(m_1 (y_0 + l_1 \cos\theta_1) + m_2 (y_0 + l_1 \cos\theta_1 + l_2 \cos\theta_2)\big) \bigg]
\end{aligned}
$$

$$\frac{\partial L}{\partial \theta_2}
= \tfrac{1}{2} m_2 \big(2 l_1 l_2 \dot\theta_1 \dot\theta_2 (-\sin(\theta_1 - \theta_2) * -1)\big)
+ g(m_2 l_2 (-\sin\theta_2))$$

$$\frac{\partial L}{\partial \theta_2}
= m_2 l_1 l_2 \dot\theta_1 \dot\theta_2 \sin(\theta_1 - \theta_2)
- g m_2 l_2 \sin\theta_2$$

Then $\partial L / \partial \dot\theta_2$:

$$\frac{\partial L}{\partial \dot\theta_2}
= \tfrac{1}{2} m_2 \big(2 l_2^2 \dot\theta_2 + 2 l_1 l_2 \dot\theta_1 \cos(\theta_1 - \theta_2)\big)$$

$$\frac{\partial L}{\partial \dot\theta_2}
= m_2 l_2^2 \dot\theta_2 + m_2 l_1 l_2 \dot\theta_1 \cos(\theta_1 - \theta_2)$$

$$
\begin{aligned}
\frac{d}{dt}\left(\frac{\partial L}{\partial \dot\theta_2}\right)
&= m_2 l_2^2 \ddot\theta_2 \\
&\quad + m_2 l_1 l_2 \big(\ddot\theta_1 \cos(\theta_1 - \theta_2) + \dot\theta_1 (-\sin(\theta_1 - \theta_2)(\dot\theta_1 - \dot\theta_2))\big)
\end{aligned}
$$

Combining and simplifying:

$$
\begin{aligned}
& m_2 l_2^2 \ddot\theta_2 + m_2 l_1 l_2 \big(\ddot\theta_1 \cos(\theta_1 - \theta_2) + \dot\theta_1 (-\sin(\theta_1 - \theta_2)(\dot\theta_1 - \dot\theta_2))\big) \\
&\quad - m_2 l_1 l_2 \dot\theta_1 \dot\theta_2 \sin(\theta_1 - \theta_2)
+ g m_2 l_2 \sin\theta_2 = 0
\end{aligned}
$$

$$
\begin{aligned}
& m_2 l_2^2 \ddot\theta_2 + m_2 l_1 l_2 \ddot\theta_1 \cos(\theta_1 - \theta_2) \\
&\quad + m_2 l_1 l_2 \dot\theta_1 (-\dot\theta_1 \sin(\theta_1 - \theta_2) + \dot\theta_2 \sin(\theta_1 - \theta_2)) \\
&\quad - m_2 l_1 l_2 \dot\theta_1 \dot\theta_2 \sin(\theta_1 - \theta_2) + g m_2 l_2 \sin\theta_2 = 0
\end{aligned}
$$

$$
\begin{aligned}
& m_2 l_2^2 \ddot\theta_2 + m_2 l_1 l_2 \ddot\theta_1 \cos(\theta_1 - \theta_2) \\
&\quad - m_2 l_1 l_2 \dot\theta_1^2 \sin(\theta_1 - \theta_2) + m_2 l_1 l_2 \dot\theta_1 \dot\theta_2 \sin(\theta_1 - \theta_2) \\
&\quad - m_2 l_1 l_2 \dot\theta_1 \dot\theta_2 \sin(\theta_1 - \theta_2) + g m_2 l_2 \sin\theta_2 = 0
\end{aligned}
$$

$$
\begin{aligned}
& m_2 l_2^2 \ddot\theta_2 + m_2 l_1 l_2 \ddot\theta_1 \cos(\theta_1 - \theta_2) \\
&\quad - m_2 l_1 l_2 \dot\theta_1^2 \sin(\theta_1 - \theta_2) + g m_2 l_2 \sin\theta_2 = 0
\end{aligned}
$$

**Euler–Lagrange equation for $\theta_2$:**

$$
m_2 l_2^2 \ddot\theta_2 + m_2 l_1 l_2 \ddot\theta_1 \cos(\theta_1 - \theta_2)
= m_2 l_1 l_2 \dot\theta_1^2 \sin(\theta_1 - \theta_2) - g m_2 l_2 \sin\theta_2
$$

### System of equations

$$
\left\{
\begin{aligned}
(m_1 + m_2) l_1^2 \ddot\theta_1 + m_2 l_1 l_2 \ddot\theta_2 \cos(\theta_1 - \theta_2) &= -g l_1 (m_1 + m_2) \sin\theta_1 - m_2 l_1 l_2 \dot\theta_2^2 \sin(\theta_1 - \theta_2) \\
m_2 l_1 l_2 \ddot\theta_1 \cos(\theta_1 - \theta_2) + m_2 l_2^2 \ddot\theta_2 &= m_2 l_1 l_2 \dot\theta_1^2 \sin(\theta_1 - \theta_2) - m_2 g l_2 \sin\theta_2
\end{aligned}
\right.
$$

$$
\left\{
\begin{aligned}
\ddot\theta_1 + \frac{m_2 l_1 l_2}{(m_1 + m_2) l_1^2}\ddot\theta_2 \cos(\theta_1 - \theta_2) &= \frac{-g l_1 (m_1 + m_2)}{(m_1 + m_2) l_1^2} \sin\theta_1 - \frac{m_2 l_1 l_2 \dot\theta_2^2}{(m_1 + m_2) l_1^2} \sin(\theta_1 - \theta_2) \\
\ddot\theta_2 + \frac{m_2 l_1 l_2}{m_2 l_2^2} \ddot\theta_1 \cos(\theta_1 - \theta_2)  &= \frac{m_2 l_1 l_2}{m_2 l_2^2} \dot\theta_1^2 \sin(\theta_1 - \theta_2) - \frac{m_2 g l_2}{m_2 l_2^2} \sin\theta_2
\end{aligned}
\right.
$$

$$
\left\{
\begin{aligned}
\\ \sigma_1&= \frac{m_2 l_1 l_2 \cos(\theta_1 - \theta_2)}{(m_1 + m_2) l_1^2} \\

\ddot\theta_1 + \sigma_1 \ddot\theta_2  &= - \frac{g}{l_1} \sin\theta_1 - \frac{\sigma_1}{\cos(\theta_1 - \theta_2)} \dot\theta_2^2 \sin(\theta_1 - \theta_2) \\

\\ \sigma_2&= \frac{m_2 l_1 l_2 \cos(\theta_1 - \theta_2)}{m_2 l_2^2} \\

\ddot\theta_2 + \sigma_2 \ddot\theta_1 &= \frac{l_1}{l_2} \dot\theta_1^2 \sin(\theta_1 - \theta_2) - \frac{\sigma_2}{\cos(\theta_1 - \theta_2)} \frac{g}{l_1}  \sin\theta_2
\end{aligned}
\right.
$$

$$
\left\{
\begin{aligned}
\sigma_1&= \frac{m_2 l_1 l_2 \cos(\theta_1 - \theta_2)}{(m_1 + m_2) l_1^2} \\
\sigma_2&= \frac{m_2 l_1 l_2 \cos(\theta_1 - \theta_2)}{m_2 l_2^2} \\
f_1(x) &= - \frac{g}{l_1} \sin\theta_1 - \frac{\sigma_1}{\cos(\theta_1 - \theta_2)} \dot\theta_2^2 \sin(\theta_1 - \theta_2) \\
f_2(x) &= \frac{l_1}{l_2} \dot\theta_1^2 \sin(\theta_1 - \theta_2) - \frac{\sigma_2}{\cos(\theta_1 - \theta_2)} \frac{g}{l_1}  \sin\theta_2 \\

\begin{pmatrix}
f_1 \\
f_2
\end{pmatrix}
&=

\begin{pmatrix}
1 & \sigma_1 \\
\sigma_2 & 1 \\
\end{pmatrix}

\begin{pmatrix}
\ddot\theta_1 \\
\ddot\theta_2 \\
\end{pmatrix}
\end{aligned}
\right.
$$

Now we need to check if that matrix is invertible

$$
\begin{aligned}
\det{
\begin{pmatrix}
1 & \sigma_1 \\
\sigma_2 & 1 \\
\end{pmatrix}
} &= 1 - \sigma_1\sigma_2 = 1 - \frac{m_2 l_1 l_2 \cos(\theta_1 - \theta_2) m_2 l_1 l_2 \cos(\theta_1 - \theta_2)}{(m_1 + m_2)l_1^2 m_2 l_2^2} \\
&= 1 - \frac{\cos(\theta_1 - \theta_2) m_2 \cos(\theta_1 - \theta_2)}{(m_1 + m_2)l_2^2} \\
&= 1 - \frac{m_2 \cos^2(\theta_1 - \theta_2)}{(m_1 + m_2)} \\
\end{aligned}
$$

First, we analyze the upper bounds of the individual components. Since physical masses are strictly positive ($m_1, m_2 > 0$), the mass ratio is strictly less than 1:
$$
0 < \frac{m_2}{m_1 + m_2} < 1
$$

Second, the square of the cosine function is naturally bounded by 1:
$$
0 \le \cos^2(\theta_1 - \theta_2) \le 1
$$

Multiplying these two terms guarantees that their product is strictly less than 1:
$$
\frac{m_2 \cos^2(\theta_1 - \theta_2)}{m_1 + m_2} < 1
$$

Therefore, subtracting this product from 1 yields a strictly positive result:
$$
1 - \frac{m_2 \cos^2(\theta_1 - \theta_2)}{m_1 + m_2} > 0
$$

This means that **the matrix is invertible**. We can do this directly:

$$
A^{-1} = 
\frac{1}{\det{A}}A
=
\frac{1}{1-\sigma_1\sigma_2}
\begin{pmatrix}
1 & -\sigma_1 \\
-\sigma_2 & 1 \\
\end{pmatrix}
$$

<br> 

$$
\begin{aligned}
\begin{pmatrix}
f_1 \\
f_2
\end{pmatrix}
&=

\begin{pmatrix}
1 & \sigma_1 \\
\sigma_2 & 1 \\
\end{pmatrix}

\begin{pmatrix}
\ddot\theta_1 \\
\ddot\theta_2 \\
\end{pmatrix}
\\
\begin{pmatrix}
\ddot\theta_1 \\
\ddot\theta_2 \\
\end{pmatrix}
&=
\begin{pmatrix}
1 & \sigma_1 \\
\sigma_2 & 1 \\
\end{pmatrix}^{-1}

\begin{pmatrix}
f_1 \\
f_2
\end{pmatrix}
\\
\begin{pmatrix}
\ddot\theta_1 \\
\ddot\theta_2 \\
\end{pmatrix}
&=
\frac{1}{1-\sigma_1\sigma_2}
\begin{pmatrix}
1 & -\sigma_1 \\
-\sigma_2 & 1 \\
\end{pmatrix}

\begin{pmatrix}
f_1 \\
f_2
\end{pmatrix}
\\
\begin{pmatrix}
\ddot\theta_1 \\
\ddot\theta_2 \\
\end{pmatrix}
&=
\frac{1}{1-\sigma_1\sigma_2}

\begin{pmatrix}
f_1 - \sigma_1 f_2 \\
f_2 - \sigma_2 f_1
\end{pmatrix}
\\

\end{aligned}
$$

So now we can write a sysmtem of differential equations for a double pendulum.\

$$\omega_1 = \dot\theta_1 \qquad \omega_2 = \dot\theta_2$$

$$
\left\{
\begin{aligned}
\sigma_1&= \frac{m_2 l_1 l_2 \cos(\theta_1 - \theta_2)}{(m_1 + m_2) l_1^2} \\
\sigma_2&= \frac{m_2 l_1 l_2 \cos(\theta_1 - \theta_2)}{m_2 l_2^2} \\
f_1(x) &= - \frac{g}{l_1} \sin\theta_1 - \frac{\sigma_1}{\cos(\theta_1 - \theta_2)} \omega_2^2 \sin(\theta_1 - \theta_2) \\
f_2(x) &= \frac{l_1}{l_2} \omega_1^2 \sin(\theta_1 - \theta_2) - \frac{\sigma_2}{\cos(\theta_1 - \theta_2)} \frac{g}{l_1}  \sin\theta_2 \\

\frac{d}{dt}
\begin{pmatrix}
\theta_1 \\
\theta_2 \\
\omega_1 \\
\omega_2 \\
\end{pmatrix}
&=
\begin{pmatrix}
\omega_1 \\
\omega_2 \\
\frac{f_1 - \sigma_1 f_2}{1 - \sigma_1\sigma_2} \\
\frac{f_2 - \sigma_2 f_1}{1 - \sigma_1\sigma_2} \\
\end{pmatrix}
\end{aligned}
\right.
$$

