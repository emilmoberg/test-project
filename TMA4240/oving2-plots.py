import numpy as np
import matplotlib.pyplot as plt

def simX(n,alpha):
    u = np.random.uniform(size=n) #array med n elementer.
    x = np.sqrt(-alpha*np.log(1-u))  # fyll inn formelen du fant for x
    
    return x



# Sett antall realisasjoner og verdien til alpha
n = 10000000
alpha = 1

# simuler realisasjoner av X
x = simX(n, alpha)

# Lag sannsynlighetshistogram for de simulerte verdiene
plt.hist(x, density=True,bins=100, color="red")  #density=True gjør at vi får et sannsynlighetshistogram

# Angi navn på aksene
plt.xlabel("x")
plt.ylabel("sim og eksakt f_X(x)") 


# Regn ut og plott sannsynlighetstettheten til X på samme plott
u = np.linspace(0,5,100)
f_x = lambda x : ((2*x)/alpha)*np.exp(-x**2/alpha)
plt.plot(u, f_x(u), color= "blue")

# Avslutt med å generere alle elementene du har plottet
plt.show()