# standard sbends
simple_testing(component-sbend               "--file=sbend.gmad"           ${OVERLAP_CHECK})
simple_testing_long(component-sbend-poleface "--file=sbend_poleface.gmad"  ${OVERLAP_CHECK})
simple_testing_long(component-sbend-fringe   "--file=sbend_fringe.gmad"    ${OVERLAP_CHECK})

# test both B and angle specified
simple_testing(component-sbend-angle-and-b   "--file=sbend_angle_and_B.gmad" ${OVERLAP_CHECK})
simple_testing(component-rbend-angle-and-b   "--file=rbend_angle_and_B.gmad" ${OVERLAP_CHECK})

# with 'bdsimtwo' integrator set - different geometry constructed
simple_testing(component-sbend-rodrigues               "--file=sbend_rodrigues.gmad"           ${OVERLAP_CHECK})
simple_testing_long(component-sbend-rodrigues-poleface "--file=sbend_rodrigues_poleface.gmad"  ${OVERLAP_CHECK})
simple_testing_long(component-sbend-rodrigues-fringe   "--file=sbend_rodrigues_fringe.gmad"    ${OVERLAP_CHECK})

# with poleface curvature using 'bdsimmatrix' integrator set
simple_testing(component-sbend-poleface-curvature   "--file=rbend_poleface_curvature.gmad"  ${OVERLAP_CHECK})

# standard rbends
simple_testing(component-rbend                   "--file=rbend.gmad"           ${OVERLAP_CHECK})
simple_testing(component-rbend-poleface          "--file=rbend_poleface.gmad"  ${OVERLAP_CHECK})
simple_testing(component-rbend-fringe            "--file=rbend_fringe.gmad"    ${OVERLAP_CHECK})

# with 'bdsimmatrix' integrator set - different geometry constructed
simple_testing(component-rbend-rodrigues            "--file=rbend_rodrigues.gmad"           ${OVERLAP_CHECK})
simple_testing(component-rbend-rodrigues-poleface   "--file=rbend_rodrigues_poleface.gmad"  ${OVERLAP_CHECK})
simple_testing(component-rbend-rodrigues-fringe     "--file=rbend_rodrigues_fringe.gmad"    ${OVERLAP_CHECK})

# with poleface curvature using 'bdsimmatrix' integrator set
simple_testing(component-rbend-poleface-curvature  "--file=sbend_poleface_curvature.gmad"  ${OVERLAP_CHECK})

# bends with quadrupole components
simple_testing(component-sbend-k1     "--file=sbend_k1.gmad"  ${OVERLAP_CHECK})
simple_testing(component-rbend-k1     "--file=rbend_k1.gmad"  ${OVERLAP_CHECK})

# strongly angled bends
simple_testing(component-rbend-strong-single-e-neg-B   "--file=rbend_strong_single_e_neg_B.gmad"   ${OVERLAP_CHECK})
simple_testing(component-rbend-strong-single-e-neg-ang "--file=rbend_strong_single_e_neg_ang.gmad" ${OVERLAP_CHECK})
simple_testing(component-rbend-strong-single-e-pos-B   "--file=rbend_strong_single_e_pos_B.gmad"   ${OVERLAP_CHECK})
simple_testing(component-rbend-strong-single-e-pos-ang "--file=rbend_strong_single_e_pos_ang.gmad" ${OVERLAP_CHECK})
simple_testing(component-rbend-strong-single-p-neg-B   "--file=rbend_strong_single_p_neg_B.gmad"   ${OVERLAP_CHECK})
simple_testing(component-rbend-strong-single-p-neg-ang "--file=rbend_strong_single_p_neg_ang.gmad" ${OVERLAP_CHECK})
simple_testing(component-rbend-strong-single-p-pos-B   "--file=rbend_strong_single_p_pos_B.gmad"   ${OVERLAP_CHECK})
simple_testing(component-rbend-strong-single-p-pos-ang "--file=rbend_strong_single_p_pos_ang.gmad" ${OVERLAP_CHECK})

simple_testing(component-sbend-strong-single-e-neg-B   "--file=sbend_strong_single_e_neg_B.gmad"   ${OVERLAP_CHECK})
simple_testing(component-sbend-strong-single-e-neg-ang "--file=sbend_strong_single_e_neg_ang.gmad" ${OVERLAP_CHECK})
simple_testing(component-sbend-strong-single-e-pos-B   "--file=sbend_strong_single_e_pos_B.gmad"   ${OVERLAP_CHECK})
simple_testing(component-sbend-strong-single-e-pos-ang "--file=sbend_strong_single_e_pos_ang.gmad" ${OVERLAP_CHECK})
simple_testing(component-sbend-strong-single-p-neg-B   "--file=sbend_strong_single_p_neg_B.gmad"   ${OVERLAP_CHECK})
simple_testing(component-sbend-strong-single-p-neg-ang "--file=sbend_strong_single_p_neg_ang.gmad" ${OVERLAP_CHECK})
simple_testing(component-sbend-strong-single-p-pos-B   "--file=sbend_strong_single_p_pos_B.gmad"   ${OVERLAP_CHECK})
simple_testing(component-sbend-strong-single-p-pos-ang "--file=sbend_strong_single_p_pos_ang.gmad" ${OVERLAP_CHECK})

# low angle with fringes
simple_testing(component-sbend-low-angle-fringe "--file=sbend_low_angle_fringe.gmad" ${OVERLAP_CHECK})
simple_testing(component-rbend-low-angle-fringe "--file=rbend_low_angle_fringe.gmad" ${OVERLAP_CHECK})

# scaled bends (no fringes)
simple_testing(component-sbend-scaled "--file=sbend_scaled.gmad" ${OVERLAP_CHECK})
simple_testing(component-rbend-scaled "--file=rbend_scaled.gmad" ${OVERLAP_CHECK})

# scaled bends with fringes with 'bdsimmatrix' integrator set
simple_testing(component-sbend-fringe-scaled "--file=sbend_fringe_scaled.gmad" ${OVERLAP_CHECK})
simple_testing(component-rbend-fringe-scaled "--file=rbend_fringe_scaled.gmad" ${OVERLAP_CHECK})

# scaled bends with fringes with 'bdsimmatrixfringescaling' integrator set - fringe kick normalised to momentum
simple_testing(component-sbend-fringescaling-scaled "--file=sbend_fringescaling_scaled.gmad" ${OVERLAP_CHECK})
simple_testing(component-rbend-fringescaling-scaled "--file=rbend_fringescaling_scaled.gmad" ${OVERLAP_CHECK})

# bends in-between thin elements with 'geant4' integrator set - test overlap of angled thin elements
simple_testing(component-sbend-thinelement-poleface "--file=sbend_thinelement_poleface.gmad" ${OVERLAP_CHECK})
simple_testing(component-rbend-thinelement-poleface "--file=rbend_thinelement_poleface.gmad" ${OVERLAP_CHECK})
