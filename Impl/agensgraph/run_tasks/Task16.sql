\timing 
  With A as
    (
        Select  latitude, longitude, avg(pm10) as pm10
        From finedust_idx
        where  timestamp >= 3 /* SF1= 3*/
        and timestamp <= 4 /* SF1 = 4*/
        group by latitude, longitude
    )
    , B as
    (  select site_id, sum((coo->>0)::FLOAT)/count(site_id) as longitude, sum((coo->>1)::FLOAT)/count(site_id) as latitude
        from
                (
                    select data->'site_id' as site_id, jsonb_array_elements(jsonb_array_elements(jsonb_array_elements(data->'geometry'->'coordinates'))) as coo
                    from site
                    where data->'properties'->>'type' = 'building' and data->'properties'->>'description'='school'
                )
               as centroid
        group by site_id
     )

Select count(*) from 
	(
    Select site_id, pm10
    from A, B
    where
        34.01189870 <= B.latitude and B.latitude <= 34.91494826
        and -118.3450100223 <= B.longitude
        and B.longitude <= -118.23192603
        and A.latitude  <= (B.latitude - 34.01189870) / 0.000172998
        and  (B.latitude - 34.01189870) / 0.000172998  <= A.latitude +  1
        and A.longitude  <= ( B.longitude - (-118.3450100223 )) / 0.000216636
        and  (B.longitude - ( -118.3450100223)) / 0.000216636  <= A.longitude +  1
	) as res ; 

