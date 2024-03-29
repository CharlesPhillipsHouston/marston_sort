# What's this?

Some python tooling for playing with TLEs

# Useful Tools
- https://github.com/ckuethe/mini_tle_server
- https://github.com/cbassa/twoline/
- https://github.com/brandon-rhodes/python-sgp4
- https://github.com/skyfielders/python-skyfield
- https://datasette.readthedocs.io/en/stable/

# IPython
- `astrometry.ipynb` - a wrapper around `solve-field` to make it easy to do astrometry of a captured image. Contains an example of it solving something.
- `skyfield satellite pass predictor.ipynb` - using skyfield to predict satellite passes. Based on [skyfield documentation](https://rhodesmill.org/skyfield/earth-satellites.html#finding-when-a-satellite-rises-and-sets)
- `MySQL DB Loader.ipynb` - storing TLEs in a database, with all kinds of fancy indexing. As a bonus, also includes some code for converting Vimpel (ISON) orbital parameters to TLE
- `geostationary_surface_point_estimator.ipynb` - an older thing using mongodb for the backing database and matplotlib to visualize things in an ipython notebook
