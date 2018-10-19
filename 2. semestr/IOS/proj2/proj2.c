#include "proj2.h"

int main(int argc, char *argv[])
{
	//Nastavení bufferu
	setbuf(stderr, NULL);

	//Ukládání parametrů
    if (saveParams(argc, argv) == 1)
    {
        fprintf(stderr, "%s \n", ERR);
        help();
        return 1;
    }
	
	//Verze 1.0
	//pid_t *adults = malloc(sizeof(pid_t)*adultCount);
	//pid_t *childs = malloc(sizeof(pid_t)*childCount);

    //Verze 2.0
	pid_t adults[adultCount];
	pid_t childs[childCount];

	if(adults == NULL || childs == NULL)
	{
		return 1;
	}

	//Odchytí zabíjecí signál
	signal(SIGTERM, sigcatch);

	//Alokace a incializace paměti / semaforů
	if(getMem() == 1)
	{
		fprintf(stderr, "Nepodarilo se alokovat pamet\n");
		clearMem();
		return 2;
	}

	pid_t adultHandler, childHandler; //ID procesů


	if((adultHandler = fork()) < 0)
	{
		fprintf(stderr, "Nepodarilo se vytvorit adultHandler\n");
		exit(2);
	}
	else if(adultHandler == 0)
	{
		for(int i = 0; i < adultCount; ++i)
		{
			usleep(AGT > 0 ? (rand()%AGT)*1000 : 0);
			adults[i] = fork();
			if(adults[i] == 0)
			{
				adultProc(i+1);
				break;
			}
		}
		//Čeká na ukončení všech procesů adult
		for(int i = 0; i < adultCount; i++)
			waitpid(adults[i], NULL, 0);
		//Uvolní paměť
		clearMem();
		exit(0);
	}
	if((childHandler = fork()) < 0)
	{
		fprintf(stderr, "Nepodarilo se vytvorit childHandler\n");
		exit(2);
	}
	else if(childHandler == 0)
	{
		for(int i = 0; i < childCount; i++)
		{
			usleep(CGT > 0 ? (rand()%CGT)*1000 : 0);
			childs[i] = fork();
			if(childs[i] == 0)
			{
				childProc(i+1);
				break;
			}
		}
		//Čeká na ukončení všech procesů child
		for(int i = 0; i < childCount; i++)
			waitpid(childs[i], NULL, 0);
		//Uvolní paměť
		clearMem();
		exit(0);
	}

	//Verze 1.0
	/*for(int i = 0; i < adultCount; i++)
	{
		int status;
		waitpid(adults[i], &status, 0);
	}
	for(int i = 0; i < childCount; i++)
	{
		int status;
		waitpid(childs[i], &status, 0);
	}*/
	//Verze 2.0
	//Čeká na ukončení potomků hlavního procesu
	waitpid(adultHandler, NULL, 0);
	waitpid(childHandler, NULL, 0);
	//Verze 1.0
	//free(childs);
	//free(adults);

	clearMem();

	return 0;

}

int adultProc(int order)
{	
	sem_wait(prior);
	sem_wait(out);
    fprintf(output, "%d\t : A %d\t : started\n",++*counter,order);
    fprintf(output, "%d\t : A %d\t : enter\n",++*counter,order);
    sem_post(out);
    ++*adultsincenter;
    release();
    if(*childstoenter >= 2)
    {
    	sem_post(child);
    	if(*childstoenter >= 3)
    		sem_post(child);
    }
    sem_post(prior);
    //TU
    usleep(AWT > 0 ? (rand()%AWT)*1000 : 0);
    sem_wait(ecla);
    sem_wait(prior);
    sem_wait(out);
    fprintf(output, "%d\t : A %d\t : trying to leave\n",++*counter,order);
    sem_post(out);
    int first = 1;
    if (((*adultsincenter-1)*3) < (*childsincenter))
	{
		++*adultstoleave;
	    sem_wait(out);
            fprintf(output, "%d\t : A %d\t : waiting : %d : %d\n",++*counter,order,*adultsincenter,*childsincenter);
        sem_post(out);
        sem_post(prior);
        first--;
        int i = 0;
    	sem_getvalue(adult, &i);
	    sem_wait(adult);
	    sem_getvalue(prior, &i);
	    sem_wait(prior);
	    --*adultstoleave;
	}
	usleep(AWT > 0 ? (rand()%AWT)*1000 : 0);
	sem_wait(out);
    fprintf(output, "%d\t : A %d\t : leave\n",++*counter,order);
    sem_post(out);
    --*remAdults;
    --*adultsincenter;
    if(*remChilds == 0 && *remAdults == 0)
   		sem_post(exitsem);
    sem_post(prior);
    sem_post(ecla);

    sem_wait(exitsem);
   	sem_wait(out);
    fprintf(output, "%d\t : A %d\t : finished\n",++*counter,order);
    sem_post(out);
   	sem_post(exitsem);


   	clearMem();
    exit(0);

}

int childProc(int order)
{
	sem_wait(prior);
	sem_wait(out);
	fprintf(output, "%d\t : C %d\t : started\n",++*counter,order);
    sem_post(out);
    sem_post(prior);

	sem_wait(ecla);
	sem_wait(prior);
	
    //if(*childsincenter == *adultsincenter*3 && *adultsincenter == 0 && *remAdults != 0)
    if(!(*remAdults == 0 || (*adultsincenter > 0 && *adultsincenter*3 > *childsincenter)))
	{
		sem_wait(out);
        fprintf(output, "%d\t : C %d\t : waiting : %d : %d\n",++*counter,order,*adultsincenter,*childsincenter);
        sem_post(out);
        //first--;
        ++*childstoenter;
        sem_post(prior);
        sem_post(ecla);
        sem_wait(child);
		sem_wait(prior);
		--*childstoenter;
	}
    sem_wait(out);
    fprintf(output, "%d\t : C %d\t : enter\n",++*counter,order);
    sem_post(out);
    ++*childsincenter;
    sem_post(prior);
    sem_post(ecla);
    usleep(CWT > 0 ? (rand()%CWT)*1000 : 0);
	sem_wait(prior);
    sem_wait(out);
    fprintf(output, "%d\t : C %d\t : trying to leave\n",++*counter,order);
    fprintf(output, "%d\t : C %d\t : leave\n",++*counter,order);
    --*remChilds;
    --*childsincenter;
    sem_post(out);
    release();
    if(*remChilds == 0 && *remAdults == 0)
   		sem_post(exitsem);
    sem_post(prior);

   	sem_wait(exitsem);
   	sem_wait(out);
    fprintf(output, "%d\t : C %d\t : finished\n",++*counter,order);
    sem_post(out);
   	sem_post(exitsem);

   	clearMem();
    exit(0);
}

void release()
{
	if(*childstoenter > 0 && (*adultsincenter)*3 > *childsincenter)
    {
    	sem_post(child);
    }

    if(*adultstoleave > 0 && (*adultsincenter-1)*3 >= *childsincenter)
    {
    	sem_post(adult);
    }
}

int getMem()
{	
	int r = 0;
	if((output = fopen("proj2.out", "w")) == NULL)
		r = 1;
	setbuf(output, NULL);
	
	//Počítání řádků
	if ((counter = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1,0)) == MAP_FAILED)
        r = 1;

    //Mapování semaforů
	if((adult = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
		r = 1;
	if((child = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
		r = 1;
	if((exitsem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
		r = 1;
	if((prior = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
		r = 1; 
	if((out = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
		r = 1; 
	if((ecla = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
		r = 1;


    //Globální proměnné 
    if ((remAdults = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1,0)) == MAP_FAILED)
        r = 1;
    if ((remChilds = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1,0)) == MAP_FAILED)
        r = 1;
    if ((adultsincenter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1,0)) == MAP_FAILED)
        r = 1;
    if ((childsincenter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1,0)) == MAP_FAILED)
        r = 1;
    if ((adultstoleave = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1,0)) == MAP_FAILED)
        r = 1;
    if ((childstoenter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1,0)) == MAP_FAILED)
        r = 1;

	//inicializace semaforů
	sem_init(ecla, 1, 1);
	sem_init(adult, 1, 0);
	sem_init(child, 1, 0);
	sem_init(exitsem, 1, 0);
	sem_init(prior, 1, 1);
	sem_init(out, 1, 1);

	*remAdults = adultCount;
	*remChilds = childCount;
	*adultsincenter = 0;
	*childsincenter = 0;
	*childstoenter = 0;
	*adultstoleave = 0;
	*counter = 0;

	return r;
}

int saveParams(int cntofparams, char *locparams[])
{
	if(cntofparams != 7)
	{
		ERR = "Spatny pocet parametru\n";
		return 1;
	}
	if(strtol(locparams[1], NULL, 10) > 0)
	{
		adultCount = strtol(locparams[1], NULL, 10);
	}
	else
	{
		ERR = "Spatny pocet dospelych\n";
		return 1;
	}
	if(strtol(locparams[2], NULL, 10) > 0)
	{
		childCount = strtol(locparams[2], NULL, 10);
	}
	else
	{
		ERR = "Spatny pocet deti\n";
		return 1;
	}
	if(strtol(locparams[3], NULL, 10) >= 0 && strtol(locparams[3], NULL, 10) < 5001)
	{
		AGT = strtol(locparams[3], NULL, 10);
	}
	else
	{
		ERR = "Spatny cas generovani dospelych\n";
		return 1;
	}
	if(strtol(locparams[4], NULL, 10) >= 0 && strtol(locparams[4], NULL, 10) < 5001)
	{
		CGT = strtol(locparams[4], NULL, 10);
	}
	else
	{
		ERR = "Spatny cas generovani deti\n";
		return 1;
	}
	if(strtol(locparams[5], NULL, 10) >= 0 && strtol(locparams[5], NULL, 10) < 5001)
	{
		AWT = strtol(locparams[5], NULL, 10);
	}
	else
	{
		ERR = "Spatny cas simulace dospelych\n";
		return 1;
	}
	if(strtol(locparams[6], NULL, 10) >= 0 && strtol(locparams[6], NULL, 10) < 5001)
	{
		CWT = strtol(locparams[6], NULL, 10);
	}
	else
	{
		ERR = "Spatny cas simulace deti\n";
		return 1;
	}
	return 0;
}

void clearMem()
{
	//Uvolneni pameti
	munmap(remAdults, sizeof(int));
	munmap(remChilds, sizeof(int));
	munmap(adultstoleave, sizeof(int));
	munmap(adultsincenter, sizeof(int));
	munmap(counter, sizeof(int));
	munmap(childsincenter, sizeof(int));
	munmap(childstoenter, sizeof(int));

	//Zniceni semaforu
	sem_destroy(adult);
	sem_destroy(child);
	sem_destroy(ecla);
	sem_destroy(prior);
	sem_destroy(exitsem);
	sem_destroy(out);

	//Zniceni semaforu
	munmap(adult, sizeof(sem_t));
	munmap(child, sizeof(sem_t));
	munmap(ecla, sizeof(sem_t));
	munmap(prior, sizeof(sem_t));
	munmap(exitsem, sizeof(sem_t));
	munmap(out, sizeof(sem_t));

	//Zavreni souboru
	fclose(output);
}

void sigcatch(int signal)
{
    signal+=1;
    clearMem();
    exit(15); //Ukončí program s SIGTERM
}

//Funkce vypisuje nápovědu na stderr v případě chyby
void help()
{
	fprintf(stderr, "A : pocet 'adult' procesu\nC : pocet 'child' procesu\n"
	"AGT : cas, ve kterem je generovan novy proces 'adult' (v ms)\n"
	"AWT : cas, po kterou proces 'adult' simuluje cinnost v centru\n"
	"CWT : cas, po kterou proces 'child' simuluje cinnost v centru\n");
}