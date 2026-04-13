class CoordinateEngine:
    """
    Translates solved physics parameters into Cartesian coordinates
    for the Qt6 C++ frontend to render vector triangles.
    """
    def generate(self, state: dict) -> dict:
        """
        Calculates the X,Y nodes of the velocity triangles.
        Y is axial (Cm), X is tangential (Cu, U).

        Args:
            state (dict): The dictionary of resolved variables from the EquationGraph.

        Returns:
            dict: JSON-serializable dictionary containing the (X, Y) coordinates
                  for the inlet and outlet velocity triangles.
        """
        # Fallback to 0.0 if a variable wasn't solved to prevent UI rendering crashes
        return {
            "inlet": {
                "origin": [0.0, 0.0],
                "c_tip": [state.get("Cu1", 0.0), state.get("Cm1", 0.0)],
                "u_tip": [state.get("U1", 0.0), 0.0]
            },
            "outlet": {
                "origin": [0.0, 0.0],
                "c_tip": [state.get("Cu2", 0.0), state.get("Cm2", 0.0)],
                "u_tip": [state.get("U2", 0.0), 0.0]
            }
        }