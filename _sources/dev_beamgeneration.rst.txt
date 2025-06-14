.. _dev-beams:

Beam Generation
***************

BDSIM uses the CLHEP random number generator and associated classes to generate
pseudo-random particle distributions for tracking.


Gauss Twiss Covariance Matrix
=============================

The beam in an accelerator can be described as a 6D distribution of particles. The six
dimensions are three canonical conjugate coordinate pairs in three planes, and can be
represented by the vector - :math:`\boldsymbol{x} = (x,\,x^{\prime},\,y,y^{\prime},\,t,\,\delta)`.
The means of that distribution are the beam centroids. The 6x6 matrix of second
order moments of the distribution is referred to as the sigma matrix and can be represented
compactly as :math:`\Sigma_{ij}=\langle\boldsymbol{x_{i}}\,\boldsymbol{x_{j}}\rangle`
with the angle brackets denoting a central moment. More explicitly for a particle beam,
this is:

.. math::

   \mathbf{\Sigma}=
   \begin{pmatrix}
   \langle x^{2}\rangle & \langle x\,x^{\prime}\rangle & \langle x\,y\rangle & \langle x\,y^{\prime}\rangle & \langle x\,t\rangle & \langle x\,\delta\rangle \\
   \langle x^{\prime}x\rangle & \langle x^{\prime 2}\rangle & \langle x^{\prime}\,y\rangle & \langle x^{\prime}\,y^{\prime}\rangle & \langle x^{\prime}\,t\rangle & \langle x^{\prime}\,\delta\rangle \\
   \langle y\,x\rangle & \langle y\,x^{\prime}\rangle & \langle y^{2}\rangle & \langle y\,y^{\prime}\rangle & \langle y\,t\rangle & \langle y\,\delta\rangle \\
   \langle y^{\prime}x\rangle & \langle y^{\prime}x^{\prime}\rangle & \langle y^{\prime}y\rangle & \langle y^{\prime 2}\rangle & \langle y^{\prime}t\rangle & \langle y^{\prime}\delta\rangle \\
   \langle t\,x\rangle & \langle t\,x^{\prime}\rangle & \langle t\,y\rangle & \langle t\,y^{\prime}\rangle & \langle t^{2}\rangle & \langle t\,\delta\rangle \\
   \langle \delta\,x\rangle & \langle \delta\,x^{\prime}\rangle & \langle \delta\,y\rangle & \langle \delta\,y^{\prime}\rangle & \langle \delta\,t\rangle & \langle \delta^{2}\rangle \\
   \end{pmatrix}


The beam is typically parameterised in an accelerator using the Twiss functions.
:math:`\beta,\alpha,\gamma` and dispersions :math:`\eta_x,\eta_x^{\prime},\eta_y,\eta_y^{\prime}`.
These are used in combination with beam parameters
:math:`\epsilon_x,\epsilon_y,\sigma_{\delta},\sigma_{t}`. The combination of
machine and beam parameters gives the physical observables such as beam size.

Please note that :math:`\sigma_{\delta}` is used here for clarity, but its definition is equivalent
to :math:`\sigma_{E}` defined in :ref:`beam-parameters`.

For linear optics, and in the absence of dispersion or energy spread, the three 2x2 submatrices
along the diagonal of the sigma matrix fully represent each plane and can be treated
independently of each other. For example, the horizontal plane is parameterised as:

.. math::
   \begin{pmatrix}
   \langle x^{2}\rangle & \langle x\,x^{\prime}\rangle \\
   \langle x\,x^{\prime}\rangle & \langle x^{\prime 2}\rangle
   \end{pmatrix}
   =
   \epsilon_{x}
   \begin{pmatrix}
   \beta_{x} & -\alpha_{x} \\
   -\alpha_{x} & \gamma_{x}
   \end{pmatrix}.

When dispersion and energy spread are present, numerous couplings are introduced,
which appear as off-diagonal elements in the sigma matrix. In this case, the energy
dependence of the coordinates results in both longitudinal-transverse and
transverse-transverse couplings.

To be a valid covariance matrix, the sigma (*capital sigma*) matrix, must be positive
definite. This ensures a positive variance in each dimension.

To derive each term of the covariance matrix, we consider the effect of dispersion
on single coordinates. For example, for an individual particle, the horizontal
position is affected as follows:

.. math::
   x_{i} = x_{\beta i} + \eta_{s}\delta_{i},

where :math:`x_{i}` is the observed coordinate, :math:`x_{\beta}` is the betatron
component and :math:`\eta_{x}\delta_{i}` reflects the change due to the particle's
relative energy offset in a dispersive region.

Because :math:`x_{\beta}` and :math:`\delta_{i}` are random variables and are
independent by construction, the second order moment in :math:`x_{\beta}` can be
factorised using the properties of the variance

.. math::
   \langle x^{2}\rangle = \mathrm{var}[x_{\beta}+\eta_{x}\delta] = \langle x_{\beta}^{2}\rangle + \eta_{x}^{2}\langle\delta_{i}^{2}\rangle = \epsilon_{x}\beta_{x} + \eta_{x}^{2}\sigma_{\delta}^{2},

where the last form is obtained by applying the Courant-Snyder equations and rewriting
the variance in terms of standard deviation.

The covariance of the horizontal position and the energy offset can also be obtained as:

.. math::
   \langle x\,\delta\rangle = \textrm{cov}[x_{\beta}+\eta_{x}\delta, \delta] = \langle x_{\beta}\,\delta\rangle + \eta_{x}\langle \delta^{2}\rangle = \eta_{x}\langle\delta^{2}\rangle,

as :math:`\langle x_{\beta}\,\delta\rangle = 0` by construction as explained above.

Finally, correlations that involve two different variables affected by dispersion
have the same form. For example:

.. math::
   \langle x\,y\rangle &= \mathrm{cov}[x_{\beta}+\eta_{x}\delta,\,x^{\prime}_{\beta}+\eta_{x^{\prime}}\delta] \\
                     &= \langle x_{\beta}\,x^{\prime}_{\beta}\rangle + \eta_{x}\langle x^{\prime}_{\beta}\,\delta\rangle + \eta_{x^{\prime}}\langle x_{\beta}\,\delta\rangle + \eta_{x}\,\eta_{x^{\prime}}\langle\delta^{2}\rangle \\
                     &= \langle x_{\beta}\,x^{\prime}_{\beta}\rangle + \eta_{x}\eta_{x^{\prime}}\langle\delta^{2}\rangle \\
                     &=-\epsilon_{x}\alpha_{x} + \eta_{x}\eta_{x^{\prime}}\sigma_{\delta}^{2}

All other couplings are done by analogy.

Some useful relations:

.. math::
   &\langle XY\rangle\equiv\mathrm{cov}[X,Y], \\
   &\langle X^2\rangle\equiv\mathrm{cov}[X,X]\equiv\mathrm{var}[X]\equiv\sigma_{X}^{2}, \\
   &\mathrm{var}[X+Y]=\mathrm{var}[X]+\mathrm{var}[Y]\,\,\mathrm{if\,\,X,Y \, are\,independent}, \\
   &\mathrm{cov}[aX+bY,cW+dV]=ac\mathrm{cov}[X,W]+ad\mathrm{cov}[X,V]+bc\mathrm{cov}[Y,W]+bd\mathrm{cov}[Y,V].
