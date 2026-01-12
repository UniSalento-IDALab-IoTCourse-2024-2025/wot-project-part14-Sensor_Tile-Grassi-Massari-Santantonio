** Compilazione **


# Creazione della cartella se non esiste
mkdir -p build      
cd build 

# Pulizia vecchi file 
rm -rf * 

# Generazione  build
cmake ..     

# Compilazione del codice
make                