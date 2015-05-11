import monitor.* ;
import java.util.Random;
// José Carlos Entrena
class Estanco extends AbstractMonitor{ 
    
    public int mostrador = -1; 
    private Condition[] ingredientes = new Condition[3]; 
    private Condition libre = makeCondition(); 
      
    public Estanco(){
        for(int i = 0; i < 3; i++)
            ingredientes[i] = makeCondition(); 
    }

    // Invocado por cada fumador, indicando su ingrediente o número
    public void obtenerIngrediente( int miIngrediente ){     
        enter(); 
        System.out.println("Entro a coger "+miIngrediente); 
	 
        if(mostrador != miIngrediente)
            ingredientes[miIngrediente].await(); 

           
        System.out.println("Salgo de coger "+miIngrediente);
        mostrador = -1;
        libre.signal(); 
        leave(); 
    }
    // Invocado por el estanquero, indicando el ingrediente que pone
    public void ponerIngrediente( int ingrediente ){ 
        enter();
        mostrador = ingrediente; 
        System.out.println("Estanco: coloco " + ingrediente); 
        ingredientes[mostrador].signal(); 
        leave(); 
    }
    // Invocado por el estanquero
    public void esperarRecogidaIngrediente(){ 
        enter(); 
        if (mostrador != -1)
            libre.await(); 
        leave(); 
    }
}

class Fumador implements Runnable{
    int miIngrediente;
    public Thread thr;
    public Estanco estanco; 
    static Random genAlea = new Random(); 

    public Fumador( int p_miIngrediente, Estanco estanco){ 
        miIngrediente = p_miIngrediente;
        this.estanco = estanco; 
        thr = new Thread(this, "Productor "+p_miIngrediente); 
    }
    
    public void run(){  
        while ( true ){ 
            estanco.obtenerIngrediente( miIngrediente );
            try{
                thr.sleep( genAlea.nextInt(2000) );
            } catch(InterruptedException e){
                System.err.println("Interrumpido al dormir"); 
            }
        }
    }
}

class Estanquero implements Runnable{ 
    public Thread thr;
    public Estanco estanco; 

    public Estanquero(Estanco estanco){
        this.estanco = estanco; 
        thr = new Thread(this, "Estanquero"); 
    }
    
    public void run(){ 
        int ingrediente ;
        while (true){
            ingrediente = (int) (Math.random () * 3.0); // 0: papel. 1: tabaco. 2: cerillas. 
            estanco.ponerIngrediente( ingrediente );
            estanco.esperarRecogidaIngrediente() ;
        }
    }
}


class Mesa{
    public static void main(String args[]){
        try{ 
            Estanco estanco = new Estanco(); 
            Estanquero pepe = new Estanquero(estanco); 
            pepe.thr.start(); 

            Fumador[] fum = new Fumador[3]; 
            
            for (int i = 0; i < 3; i++){
                fum[i] = new Fumador(i, estanco); 
                fum[i].thr.start(); 
            }
            
            pepe.thr.join(); fum[0].thr.join(); fum[1].thr.join(); fum[2].thr.join(); 

        }
        catch (Exception e){ 
            System.out.println ("Excepción.");
        }
    }
}
