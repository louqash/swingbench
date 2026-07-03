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
