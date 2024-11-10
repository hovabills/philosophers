/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adouiyeh <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 02:05:15 by adouiyeh          #+#    #+#             */
/*   Updated: 2024/10/28 02:05:18 by adouiyeh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	philo_eat(t_philo *philo)
{
	pthread_mutex_lock(&(philo->table->forks[philo->left_fork_id]));
	print_action(philo->table, philo->id, "has taken a fork");
	pthread_mutex_lock(&(philo->table->forks[philo->right_fork_id]));
	print_action(philo->table, philo->id, "has taken a fork");
	pthread_mutex_lock(&(philo->table->meal_check));
	print_action(philo->table, philo->id, "is eating");
	philo->last_meal = timestamp();
	pthread_mutex_unlock(&(philo->table->meal_check));
	(philo->n_meals)++;
	safe_sleep(philo->table->time_to_eat, philo->table);
	pthread_mutex_unlock(&(philo->table->forks[philo->left_fork_id]));
	pthread_mutex_unlock(&(philo->table->forks[philo->right_fork_id]));
}

void	watch_dog(t_table *table, t_philo *philos)
{
	int	i;

	i = 0;
	while (!(table->all_full) && !(table->deaths))
	{
		pthread_mutex_lock(&(table->meal_check));
		if (time_diff(table->philos[i].last_meal,
				timestamp()) > table->time_to_die)
		{
			print_action(table, i, "died");
			table->deaths = 1;
		}
		pthread_mutex_unlock(&(table->meal_check));
		usleep(100);
		if (table->deaths)
			break ;
		i = 0;
		while (table->n_meals != -1 && i < table->n_philos
			&& table->n_meals <= philos[i].n_meals)
			i++;
		if (i == table->n_philos)
			table->all_full = 1;
	}
}

void	exit_sim(t_table *table, t_philo *philos)
{
	int	i;

	i = 1;
	while (i < table->n_philos)
	{
		pthread_join(philos[i].thread, NULL);
		i++;
	}
	i = 1;
	while (i < table->n_philos)
	{
		pthread_mutex_destroy(&(table->forks[i]));
		i++;
	}
	pthread_mutex_destroy(&(table->write_lock));
	pthread_mutex_destroy(&(table->meal_check));
}

int	start_sim(t_table *table)
{
	int		i;
	t_philo	*philo;

	i = 0;
	table->start_time = timestamp();
	while (i < table->n_philos)
	{
		if (pthread_create(&(table->philos[i].thread), NULL, philo_start,
				&(table->philos[i])))
			return (1);
		table->philos[i].last_meal = timestamp();
		i++;
	}
	watch_dog(table, table->philos);
	exit_sim(table, table->philos);
	return (0);
}

int	main(int ac, char **av)
{
	t_table	table;

	if (ac != 5 && ac != 6)
		return (printf("Wrong number of args"));
	if (!(is_args_valid(av)))
		return (printf("Args are not valid"));
	if (table_init(&table, av))
		return (printf("Mutex init failure"));
	philos_init(&table);
	if (start_sim(&table))
		return (printf("Thread creation failure"));
}
