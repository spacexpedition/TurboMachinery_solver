import sympy as sp

class EquationGraph:
    """
    Symbolic math engine that traverses turbomachinery equations.
    """
    def __init__(self):
        # Velocities: U (Blade), C (Absolute), W (Relative)
        self.U1, self.U2 = sp.symbols('U1 U2', real=True)
        self.C1, self.C2 = sp.symbols('C1 C2', real=True, positive=True)
        self.W1, self.W2 = sp.symbols('W1 W2', real=True, positive=True)
        self.Cm1, self.Cm2 = sp.symbols('Cm1 Cm2', real=True, positive=True)
        self.Cu1, self.Cu2 = sp.symbols('Cu1 Cu2', real=True)
        self.Wu1, self.Wu2 = sp.symbols('Wu1 Wu2', real=True)

        # Angles (radians)
        self.alpha1, self.alpha2 = sp.symbols('alpha1 alpha2', real=True)
        self.beta1, self.beta2 = sp.symbols('beta1 beta2', real=True)

        # Thermodynamics
        self.w = sp.symbols('w', real=True) # Specific work
        self.power = sp.symbols('power', real=True)
        self.m_dot = sp.symbols('m_dot', real=True, positive=True)

        self.equations = [
            # Inlet
            sp.Eq(self.C1**2, self.Cm1**2 + self.Cu1**2),
            sp.Eq(self.W1**2, self.Cm1**2 + self.Wu1**2),
            sp.Eq(self.U1, self.Cu1 - self.Wu1),
            sp.Eq(sp.tan(self.alpha1), self.Cu1 / self.Cm1),
            sp.Eq(sp.tan(self.beta1), self.Wu1 / self.Cm1),
            # Outlet
            sp.Eq(self.C2**2, self.Cm2**2 + self.Cu2**2),
            sp.Eq(self.W2**2, self.Cm2**2 + self.Wu2**2),
            sp.Eq(self.U2, self.Cu2 - self.Wu2),
            sp.Eq(sp.tan(self.alpha2), self.Cu2 / self.Cm2),
            sp.Eq(sp.tan(self.beta2), self.Wu2 / self.Cm2),
            # Euler Work
            sp.Eq(self.w, self.U1 * self.Cu1 - self.U2 * self.Cu2),
            sp.Eq(self.power, self.m_dot * self.w)
        ]
        self.symbol_map = {sym.name: sym for sym in self.__dict__.values() if isinstance(sym, sp.Symbol)}

    def solve(self, knowns_dict):
        resolved = {self.symbol_map[k]: v for k, v in knowns_dict.items() if k in self.symbol_map}
        unresolved_eqs = self.equations.copy()
        progress_made = True

        while progress_made and unresolved_eqs:
            progress_made = False
            remaining_eqs = []
            for eq in unresolved_eqs:
                subbed_eq = eq.subs(resolved)
                free_symbols = subbed_eq.free_symbols
                if len(free_symbols) == 1:
                    target_symbol = list(free_symbols)[0]
                    try:
                        solutions = sp.solve(subbed_eq, target_symbol)
                        if solutions:
                            resolved[target_symbol] = float(abs(solutions[0]))
                            progress_made = True
                        else:
                            remaining_eqs.append(eq)
                    except Exception:
                        remaining_eqs.append(eq)
                elif len(free_symbols) > 0:
                    remaining_eqs.append(eq)
            unresolved_eqs = remaining_eqs

        return {sym.name: round(val, 4) for sym, val in resolved.items()}