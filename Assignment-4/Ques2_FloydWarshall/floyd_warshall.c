#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAXN 101
#define INF 100000000

int Graph[MAXN][MAXN];
int dist[MAXN][MAXN];

pthread_mutex_t mutex,wrt;
int readCount;
int n;


void * handle_i(void *args)
{
	int i = ((int *)(args))[0];
	int k = ((int *)(args))[1];
	int j,dist_i_k,dist_k_j,dist_i_j;

	for(j=1;j<=n;++j)
	{
		// Read dist[i][k], dist[k][j] and dist[i][j]
		pthread_mutex_lock(&mutex);
		
		readCount++;
		if(readCount == 1)
		{
			pthread_mutex_lock(&wrt);
		}

		pthread_mutex_unlock(&mutex);

		dist_i_k = dist[i][k];
		dist_k_j = dist[k][j];
		dist_i_j = dist[i][j];

		pthread_mutex_lock(&mutex);

		readCount--;

		if(readCount == 0)
		{
			pthread_mutex_unlock(&wrt);
		}

		pthread_mutex_unlock(&mutex);

		if(dist_i_k+dist_k_j<dist_i_j)
		{
			// Write dist[i][j]	
			pthread_mutex_lock(&wrt);
			dist[i][j] = dist_i_k+dist_k_j;
			pthread_mutex_unlock(&wrt);
		}
	}
}

int main()
{
	int i,j,k,m,u,v,w;
	pthread_t threads[MAXN];
	int arg[MAXN][2];

	// Take N & M as input 

	scanf("%d%d",&n,&m);
	if(n == 0 || m == 0){
		return  0;
	}
	// initialiaze the Adjacency matrix

	for(i=1;i<=n;++i)
	{
		for(j=1;j<=n;++j)
		{
			Graph[i][j] = 0;
		}
	}

	// Input edges

	for(i=0;i<m;++i)
	{
		scanf("%d%d%d",&u,&v,&w);
		Graph[u][v] = w;
		Graph[v][u] = w;
	}

	// Initialiaze dist Matrix to Adjacency

	for(i=1;i<=n;++i)
	{
		for(j=1;j<=n;++j)
		{
			if(i == j) dist[i][j] = 0;
			else if(Graph[i][j] == 0)
			{
				dist[i][j] = INF;
			}
			else{
				dist[i][j] = Graph[i][j];
			}
		}
	}

	printf("\nInitial dist Matrix\n");
	printf("------------------\n\n");

	for(i=1;i<=n;++i)
	{
		for(j=1;j<=n;++j)
		{
			if(dist[i][j] == INF)
			{
				printf("INF\t");
			}
			else printf("%d\t",dist[i][j]);
		}
		printf("\n");
	}
	

	// Use Floyd Warshall to compute All Pair Shortest Path Distance

	for(k=1;k<=n;++k)
	{
		// Initialize mutex and wrt

		pthread_mutex_init(&mutex,NULL);
		pthread_mutex_init(&wrt,NULL);
		readCount = 0;

		// Create n Threads
		for(i=1;i<=n;++i)
		{
			arg[i][0] = i;
			arg[i][1] = k;

			if(pthread_create(&threads[i],NULL,handle_i,&(arg[i])))
			{
				printf("Error Creating Thread\n");
			}
		}


		// Wait for n threads to finish
		for(i=1;i<=n;++i)
		{
			if(pthread_join(threads[i],NULL))
			{
				printf("Error Joining Thread\n");
			}
		}

		// Destroy mutext and wrt

		pthread_mutex_destroy(&mutex);
		pthread_mutex_destroy(&wrt);
	}

	printf("\nFinal dist Matrix\n");
	printf("------------------\n");

	for(i=1;i<=n;++i)
	{
		for(j=1;j<=n;++j)
		{
			if(dist[i][j] == INF)
			{
				printf("INF\t");
			}
			else printf("%d\t",dist[i][j]);
		}
		printf("\n");
	}

	return 0;
}